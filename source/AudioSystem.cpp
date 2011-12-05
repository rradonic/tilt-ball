/*
This file is part of TiltBall.
http://github.com/rradonic/tilt-ball

Copyright (C) 2009-2011 Ranko Radonić

TiltBall is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

TiltBall is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with TiltBall.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "AudioSystem.hpp"

#include <AL/al.h>
#include <AL/alut.h>
#include <iostream>
#include <vorbis/vorbisfile.h>

namespace TiltBall
{
    AudioSystem::AudioSystem()
    {
        // decode ogg music
        OggVorbis_File vorbis;
        ov_fopen("../resources/sound/music.ogg", &vorbis);

        vorbis_info* info = ov_info(&vorbis, -1);
        ALenum format = info->channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
        ALsizei rate = info->rate;

        int bitstream;
        char tmp[32768];
        int bytesRead;

        do
        {
            bytesRead = ov_read(&vorbis, tmp, 32768, 0, 2, 1, &bitstream);
            m_vorbisBuffer.insert(m_vorbisBuffer.end(), tmp, tmp + bytesRead);
        }
        while(bytesRead > 0);

        ov_clear(&vorbis);

        std::clog << m_vorbisBuffer.size() << " bytes of music read" << std::endl;

        alutInit(0, 0);

        // import music into openal and starting playing
        alGenBuffers(1, &m_musicBuffer);
        alGenSources(1, &m_musicSource);

        alBufferData(m_musicBuffer,
                     format,
                     &m_vorbisBuffer[0],
                     static_cast<ALsizei>(m_vorbisBuffer.size()),
                     rate);

        alSourcei(m_musicSource, AL_BUFFER, m_musicBuffer);
        alSourcef(m_musicSource, AL_GAIN, 0.3f);

        alSourcePlay(m_musicSource);

        // import click sound into openal
        m_clickSoundBuffer = alutCreateBufferFromFile("../resources/sound/click.wav");
        alGenSources(1, &m_clickSoundSource);
        alSourcei(m_clickSoundSource, AL_BUFFER, m_clickSoundBuffer);
    }

    AudioSystem::~AudioSystem()
    {
        alDeleteBuffers(1, &m_musicBuffer);
        alDeleteSources(1, &m_musicSource);
        alDeleteBuffers(1, &m_clickSoundBuffer);
        alDeleteSources(1, &m_clickSoundSource);
        alutExit();
    }
}
