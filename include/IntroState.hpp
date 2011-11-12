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

#ifndef INTROSTATE_HPP
#define INTROSTATE_HPP

#include "Engine.hpp"
#include "GameState.hpp"

#include <ctime>

namespace TiltBall
{
    class IntroState: public GameState
    {
    public:
        explicit IntroState(Engine* p_engine);

        void pause();

        void resume();

        bool update(const Ogre::FrameEvent& p_event);

        bool mouseMoved(const OIS::MouseEvent &evt);

        bool mousePressed(const OIS::MouseEvent &evt,
                          OIS::MouseButtonID);

        bool mouseReleased(const OIS::MouseEvent &evt,
                           OIS::MouseButtonID);

        bool keyPressed(const OIS::KeyEvent &evt);

        bool keyReleased(const OIS::KeyEvent &evt);

    private:
        unsigned long m_totalMilliseconds;
        unsigned long m_elapsedMilliseconds;
        Ogre::Timer m_timer;

        Ogre::TextureUnitState* m_textureUnitState;
        Ogre::Overlay* m_fadeOverlay;
        float m_alpha;
    };
}

#endif
