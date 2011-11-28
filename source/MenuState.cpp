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

#include "MenuState.hpp"

namespace TiltBall
{
    MenuState::MenuState(Engine* p_engine) :
        GameState(p_engine)
    {
        std::clog << "Entering menu state..." << std::endl;

        CEGUI::WindowManager& windowManager = CEGUI::WindowManager::getSingleton();

        CEGUI::Window* layout = windowManager.loadWindowLayout("main-menu.layout");
        CEGUI::System::getSingleton().setGUISheet(layout);
        unsigned int width, height, colorDepth;
        m_engine->getOgreRoot()->getRenderTarget("main_window")->getMetrics(width,
                                                                            height,
                                                                            colorDepth);
        CEGUI::MouseCursor::getSingleton().show();

        CEGUI::PushButton* resumeButton =
            (CEGUI::PushButton*)windowManager.getWindow("Menu/Resume");
        resumeButton->subscribeEvent(CEGUI::PushButton::EventClicked,
                                     CEGUI::SubscriberSlot(&MenuState::onResumeButtonClicked, this));

        CEGUI::PushButton* quitButton = (CEGUI::PushButton*)windowManager.getWindow("Menu/Quit");
        quitButton->subscribeEvent(CEGUI::PushButton::EventClicked,
                                   CEGUI::SubscriberSlot(&MenuState::onQuitButtonClicked, this));
    }

    MenuState::~MenuState()
    {
        std::clog << "MenuState destructor!" << std::endl;

        CEGUI::MouseCursor::getSingleton().hide();
        CEGUI::System::getSingleton().getGUISheet()->hide();
        CEGUI::WindowManager::getSingleton().destroyAllWindows();
    }

    void MenuState::pause()
    {
    }

    void MenuState::resume()
    {
    }

    bool MenuState::update(const Ogre::FrameEvent& p_event)
    {
        InputSystem* inputSystem = m_engine->getInputSystem();
        inputSystem->capture();

        return true;
    }

    bool MenuState::mouseMoved(const OIS::MouseEvent& p_evt)
    {
        CEGUI::System::getSingleton().injectMouseMove(p_evt.state.X.rel, p_evt.state.Y.rel);
        if (p_evt.state.Z.rel)
            CEGUI::System::getSingleton().injectMouseWheelChange(p_evt.state.Z.rel / 120.0f);

        return true;
    }

    bool MenuState::mousePressed(const OIS::MouseEvent& p_evt, OIS::MouseButtonID p_buttonId)
    {
        CEGUI::System::getSingleton().injectMouseButtonDown(convertMouseButton(p_buttonId));
        return true;
    }

    bool MenuState::mouseReleased(const OIS::MouseEvent& p_evt, OIS::MouseButtonID p_buttonId)
    {
        CEGUI::System::getSingleton().injectMouseButtonUp(convertMouseButton(p_buttonId));
        return true;
    }

    bool MenuState::keyPressed(const OIS::KeyEvent& p_evt)
    {
         CEGUI::System::getSingleton().injectKeyDown(p_evt.key);
         CEGUI::System::getSingleton().injectChar(p_evt.text);

         if (p_evt.key == OIS::KC_ESCAPE)
             m_engine->requestPop();

        return true;
    }

    bool MenuState::keyReleased(const OIS::KeyEvent& p_evt)
    {
        return true;
    }

    CEGUI::MouseButton MenuState::convertMouseButton(OIS::MouseButtonID p_buttonId)
    {
        switch (p_buttonId)
        {
        case OIS::MB_Left:
            return CEGUI::LeftButton;

        case OIS::MB_Right:
            return CEGUI::RightButton;

        case OIS::MB_Middle:
            return CEGUI::MiddleButton;

        default:
            return CEGUI::LeftButton;
        }
    }

    bool MenuState::onResumeButtonClicked(const CEGUI::EventArgs& e)
    {
        std::clog << "Resume clicked!" << std::endl;
        m_engine->requestPop();
        return true;
    }

    bool MenuState::onQuitButtonClicked(const CEGUI::EventArgs& e)
    {
        std::clog << "Quit clicked!" << std::endl;
        m_engine->requestQuit();
        return true;
    }
}
