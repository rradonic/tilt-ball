/*
This file is part of TiltBall.
http://sourceforge.net/projects/tiltball

Copyright (C) 2009 Ranko Radonić

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

#ifndef RUNNINGSTATE_HPP
#define	RUNNINGSTATE_HPP

#include "Engine.hpp"
#include "GameState.hpp"

namespace TiltBall
{
    class Level;

    class RunningState: public GameState
    {
    public:
        explicit RunningState(Engine* p_engine);
        RunningState(const RunningState& p_other) = delete;
        RunningState& operator=(const RunningState& p_other) = delete;
        ~RunningState();

        void pause();
        void resume();

        bool update(const Ogre::FrameEvent& p_event);

        bool mouseMoved(const OIS::MouseEvent &evt);
        bool mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID);
        bool mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID);

        bool keyPressed(const OIS::KeyEvent &evt);
        bool keyReleased(const OIS::KeyEvent &evt);

    private:
        // heap object because we will want to allocate and destroy Level objects as we go from
        // level to level in the game
        Level* m_currentLevel;
    };
}

#endif
