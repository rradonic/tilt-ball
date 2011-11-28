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

#include "InputSystem.hpp"

namespace TiltBall
{
    InputSystem::InputSystem(Ogre::Root* p_ogreRoot) :
        m_inputManager(createInputSystem(p_ogreRoot)),
        m_keyboard(dynamic_cast<OIS::Keyboard*>(m_inputManager->
                                                createInputObject(OIS::OISKeyboard, true))),
        m_mouse(dynamic_cast<OIS::Mouse*>(m_inputManager->createInputObject(OIS::OISMouse, true)))
    {
        unsigned int width, height, depth;
        p_ogreRoot->getRenderTarget("main_window")->getMetrics(width, height, depth);
        const OIS::MouseState& ms = m_mouse->getMouseState();
        ms.width = width;
        ms.height = height;
    }

    InputSystem::~InputSystem()
    {
        std::clog << "Shutting down input system..." << std::endl;
        m_inputManager->destroyInputObject(m_mouse);
        m_inputManager->destroyInputObject(m_keyboard);
        OIS::InputManager::destroyInputSystem(m_inputManager);
    }

    OIS::InputManager* InputSystem::createInputSystem(Ogre::Root* p_root)
    {
        std::clog << "Initializing input system..." << std::endl;
        OIS::ParamList paramList;
        size_t windowHandle = 0;
        std::ostringstream windowHandleStr;
        p_root->getRenderTarget("main_window")->getCustomAttribute("WINDOW", &windowHandle);
        windowHandleStr << windowHandle;
        paramList.insert(std::make_pair(std::string("WINDOW"), windowHandleStr.str()));

        return OIS::InputManager::createInputSystem(paramList);
    }

    bool InputSystem::isKeyDown(OIS::KeyCode p_key)
    {
        return m_keyboard->isKeyDown(p_key);
    }

    const OIS::MouseState& InputSystem::getMouseState()
    {
        return m_mouse->getMouseState();
    }

    void InputSystem::capture()
    {
        m_keyboard->capture();
        m_mouse->capture();
    }

    OIS::Keyboard* InputSystem::getKeyboard()
    {
        return m_keyboard;
    }

    OIS::Mouse* InputSystem::getMouse()
    {
        return m_mouse;
    }
}
