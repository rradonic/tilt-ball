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

#include <AL/al.h>
#include <AL/alut.h>
#include <vorbis/vorbisfile.h>

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
        light->setPosition(0, 20, 0);
        light->setDiffuseColour(Ogre::ColourValue(0.1, 0.1, 0.1));
        light->setSpecularColour(Ogre::ColourValue(0.1, 0.1, 0.1));

        // decode ogg music
        ov_fopen("../resources/sound/music.ogg", &m_vorbis);

        vorbis_info *info = ov_info(&m_vorbis, -1);
        ALenum format = info->channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
        ALsizei rate = info->rate;

        int bitstream;
        char tmp[32768];
        int bytesRead;

        do
        {
            bytesRead = ov_read(&m_vorbis, tmp, 32768, 0, 2, 1, &bitstream);
            m_vorbisBuffer.insert(m_vorbisBuffer.end(), tmp, tmp + bytesRead);
        }
        while(bytesRead > 0);

        ov_clear(&m_vorbis);

        std::clog << m_vorbisBuffer.size() << " bytes of music read" << std::endl;

        // import music into openal and starting playing
        alutInit(0, 0);

        alGenBuffers(1, &m_openalBuffer);
        alGenSources(1, &m_openalSource);

        alBufferData(m_openalBuffer,
                     format,
                     &m_vorbisBuffer[0],
                     static_cast<ALsizei>(m_vorbisBuffer.size()),
                     rate);

        alSourcei(m_openalSource, AL_BUFFER, m_openalBuffer);
        alSourcePlay(m_openalSource);
    }

    RunningState::~RunningState()
    {
        delete m_currentLevel;

        alDeleteBuffers(1, &m_openalBuffer);
        alDeleteSources(1, &m_openalSource);
        alutExit();
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
            m_engine->getDynamicsWorld()->debugDrawWorld();

        Ogre::SceneNode *levelNode = m_currentLevel->getLevelNode();
        Ogre::SceneNode *targetNode = m_currentLevel->getTargetNode();

        const OIS::MouseState &mouseState = inputSystem->getMouseState();

        // move the level
        levelNode->roll(Ogre::Degree(-(float)mouseState.X.rel / 7), Ogre::Node::TS_LOCAL);
        levelNode->pitch(Ogre::Degree((float)mouseState.Y.rel / 7), Ogre::Node::TS_WORLD);

        btRigidBody *levelBody = m_currentLevel->getLevelBody();
        OgreMotionState *levelMotionState =
            dynamic_cast<OgreMotionState*>(levelBody->getMotionState());
        btTransform newBtLevelTransform = btTransform(btQuaternion(levelNode->getOrientation().x,
                                                                   levelNode->getOrientation().y,
                                                                   levelNode->getOrientation().z,
                                                                   levelNode->getOrientation().w));
        levelMotionState->kinematicSetPosition(newBtLevelTransform);

        // move the target
        btRigidBody *targetBody = m_currentLevel->getTargetBody();
        OgreMotionState *targetMotionState =
            dynamic_cast<OgreMotionState*>(targetBody->getMotionState());

        Ogre::Vector3 targetWorldPosition =
            targetNode->_getDerivedPosition();

        btTransform newBtTargetTransform =
            btTransform(btQuaternion(targetNode->getOrientation().x,
                                     targetNode->getOrientation().y,
                                     targetNode->getOrientation().z,
                                     targetNode->getOrientation().w),
                        btVector3(targetWorldPosition.x,
                                  targetWorldPosition.y,
                                  targetWorldPosition.z));

        targetMotionState->kinematicSetPosition(newBtTargetTransform);

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

    void RunningState::loadNextLevel()
    {
        std::string nextLevelFileName = m_currentLevel->getNextLevelFileName();

        std::fstream stream(nextLevelFileName);
        if(!stream.good())
        {
            m_engine->requestQuit();
            return;
        }
        stream.close();

        delete m_currentLevel;

        m_currentLevel = new Level(m_engine, nextLevelFileName);
    }
}
