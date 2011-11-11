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

#include "IntroState.hpp"

namespace TiltBall
{
    IntroState::IntroState(Engine* p_engine) :
        GameState(p_engine),
        m_totalMilliseconds(std::time_t(2000)),
        m_elapsedMilliseconds(0)
    {
        std::clog << "Entering intro state..." << std::endl;

        // do one input state capture just to hide the mouse cursor
        InputSystem* inputSystem = m_engine->getInputSystem();
        inputSystem->capture();

        // get the material by name
        std::clog << "Loading fade overlay material..." << std::endl;
        Ogre::ResourcePtr resptr = Ogre::MaterialManager::getSingleton().
            getByName("Materials/FadeOverlay");
        Ogre::Material* material = dynamic_cast<Ogre::Material*>(resptr.getPointer());
        Ogre::Technique* tech = material->getTechnique(0);
        Ogre::Pass *pass = tech->getPass(0);

        m_textureUnitState = pass->getTextureUnitState(0);

        // get the overlay
        std::clog << "Loading fade overlay..." << std::endl;
        m_fadeOverlay = Ogre::OverlayManager::getSingleton().getByName("Overlays/FadeOverlay");

        m_alpha = 1.0;
        m_fadeOverlay->show();
    }

    void IntroState::pause()
    {
    }

    void IntroState::resume()
    {
    }

    bool IntroState::update(const Ogre::FrameEvent& p_event)
    {
        m_elapsedMilliseconds = m_timer.getMilliseconds();

        if (m_elapsedMilliseconds > m_totalMilliseconds)
        {
            m_fadeOverlay->hide();
            m_engine->requestPop();
        }

        if(m_textureUnitState)
        {
            // Set the m_alpha value of the m_fadeOverlay
            m_textureUnitState->setAlphaOperation(Ogre::LBX_MODULATE, Ogre::LBS_MANUAL,
                Ogre::LBS_TEXTURE, m_alpha);    // Change the m_alpha operation

            m_alpha = 1.0 - (float)m_elapsedMilliseconds / (float)m_totalMilliseconds;
        }

        return true;
    }

    bool IntroState::mouseMoved(const OIS::MouseEvent &evt)
    {
        return true;
    }

    bool IntroState::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID)
    {
        return true;
    }

    bool IntroState::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID)
    {
        return true;
    }

    bool IntroState::keyPressed(const OIS::KeyEvent &evt)
    {
        return true;
    }

    bool IntroState::keyReleased(const OIS::KeyEvent &evt)
    {
        return true;
    }
}
