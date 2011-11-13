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

#ifndef MENUSTATE_HPP
#define	MENUSTATE_HPP

#include "Engine.hpp"
#include "GameState.hpp"

namespace TiltBall
{
    class MenuState: public GameState
    {
    public:
        explicit MenuState(Engine *p_engine);

        MenuState(const MenuState &p_other) = delete;

        MenuState &operator=(const MenuState &p_other) = delete;

        ~MenuState();

        void pause();

        void resume();

        bool update(const Ogre::FrameEvent &p_event);

        bool mouseMoved(const OIS::MouseEvent &p_evt);

        bool mousePressed(const OIS::MouseEvent &p_evt,
                          OIS::MouseButtonID p_buttonId);

        bool mouseReleased(const OIS::MouseEvent &p_evt,
                           OIS::MouseButtonID p_buttonId);

        bool keyPressed(const OIS::KeyEvent &p_evt);
        bool keyReleased(const OIS::KeyEvent &p_evt);

    private:
        CEGUI::MouseButton convertMouseButton(OIS::MouseButtonID p_buttonId);

        bool onResumeButtonClicked(const CEGUI::EventArgs &e);

        bool onQuitButtonClicked(const CEGUI::EventArgs &e);

        bool m_quit;
    };
}

#endif
