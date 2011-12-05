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

#ifndef AUDIOSYSTEM_HPP
#define	AUDIOSYSTEM_HPP

#include <AL/al.h>
#include <vector>

namespace TiltBall
{
    class AudioSystem
    {
    public:
        AudioSystem();

        AudioSystem(const AudioSystem& p_other) = delete;

        AudioSystem& operator=(const AudioSystem& p_other) = delete;

        ~AudioSystem();

    private:
        std::vector<char> m_vorbisBuffer;

        ALuint m_musicBuffer;
        ALuint m_musicSource;
        ALuint m_clickSoundBuffer;
        ALuint m_clickSoundSource;
    };
}

#endif
