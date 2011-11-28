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

#ifndef INPUTSYSTEM_HPP
#define INPUTSYSTEM_HPP

#include <OGRE/Ogre.h>
#include <OIS/OIS.h>

namespace TiltBall
{
    class InputSystem
    {
    public:
        explicit InputSystem(Ogre::Root* p_root);

        InputSystem(const InputSystem& p_other) = delete;

        InputSystem& operator=(const InputSystem& p_other) = delete;

        ~InputSystem();

        void capture();

        const OIS::MouseState& getMouseState();

        bool isKeyDown(OIS::KeyCode p_key);

        OIS::Keyboard* getKeyboard();

        OIS::Mouse* getMouse();

    private:
        OIS::InputManager* createInputSystem(Ogre::Root* p_root);

        OIS::InputManager* m_inputManager;
        OIS::Keyboard* m_keyboard;
        OIS::Mouse* m_mouse;
    };
}

#endif
