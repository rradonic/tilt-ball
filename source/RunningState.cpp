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

#include "RunningState.hpp"
#include "MenuState.hpp"
#include "Level.hpp"
#include "OgreMotionState.hpp"

namespace TiltBall
{
    RunningState::RunningState(Engine *p_engine) :
        GameState(p_engine),
        m_currentLevel(new Level(p_engine, "../resources/levels/level1.lvl"))
    {
        std::clog << "Entering running state..." << std::endl;
        Ogre::SceneManager *sceneManager = m_engine->getOgreRoot()->
            getSceneManager("main_scene_manager");

        std::clog << "Setting up lighting..." << std::endl;
        sceneManager->setAmbientLight(Ogre::ColourValue(0.8, 0.8, 0.8));
        Ogre::Light *light = sceneManager->createLight("main_light");
        light->setType(Ogre::Light::LT_POINT);
        light->setPosition(0, 10, 0);
        light->setDiffuseColour(Ogre::ColourValue(0.5, 0.5, 0.5));
        light->setSpecularColour(Ogre::ColourValue(0.3, 0.3, 0.3));
    }

    RunningState::~RunningState()
    {
        delete m_currentLevel;
    }

    void RunningState::pause()
    {
    }

    void RunningState::resume()
    {
    }

    bool RunningState::update(const Ogre::FrameEvent &p_event)
    {
        m_engine->getDynamicsWorld()->stepSimulation(m_engine->getTimeSinceLastFrame());

        InputSystem *inputSystem = m_engine->getInputSystem();
        inputSystem->capture();

        m_engine->getDebugDrawer()->clear();
        if(inputSystem->isKeyDown(OIS::KC_F1))
        {
            m_engine->getDynamicsWorld()->debugDrawWorld();
        }

        Ogre::SceneNode *levelNode = m_currentLevel->getLevelNode();

        const OIS::MouseState &mouseState = inputSystem->getMouseState();

        levelNode->roll(Ogre::Degree(-(float)mouseState.X.rel / 7), Ogre::Node::TS_LOCAL);
        levelNode->pitch(Ogre::Degree((float)mouseState.Y.rel / 7), Ogre::Node::TS_WORLD);

        btRigidBody *levelBody = m_currentLevel->getLevelBody();
        OgreMotionState *levelMotionState = dynamic_cast<OgreMotionState*>(levelBody->getMotionState());
        btTransform newPhysicsPosition = btTransform(btQuaternion(levelNode->getOrientation().x,
                                                                  levelNode->getOrientation().y,
                                                                  levelNode->getOrientation().z,
                                                                  levelNode->getOrientation().w));
        levelMotionState->kinematicSetPosition(newPhysicsPosition);

        return true;
    }

    bool RunningState::mouseMoved(const OIS::MouseEvent &evt)
    {
        return true;
    }

    bool RunningState::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID)
    {
        return true;
    }

    bool RunningState::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID)
    {
        return true;
    }

    bool RunningState::keyPressed(const OIS::KeyEvent &evt)
    {
        if (evt.key == OIS::KC_Q)
            m_engine->requestQuit();
        if (evt.key == OIS::KC_ESCAPE)
            m_engine->pushState(new MenuState(m_engine));

        return true;
    }

    bool RunningState::keyReleased(const OIS::KeyEvent &evt)
    {
        return true;
    }
}
