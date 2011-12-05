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

#ifndef ENGINE_HPP
#define ENGINE_HPP

#include "BulletDebugDrawer.hpp"
#include "GameState.hpp"
#include "InputSystem.hpp"

#include <btBulletDynamicsCommon.h>
#include <CEGUI/CEGUI.h>
#include <OGRE/Ogre.h>
#include <CEGUI/RendererModules/Ogre/CEGUIOgreRenderer.h>
#include <OIS/OIS.h>
#include <vector>

namespace TiltBall
{
    class AudioSystem;
    class BulletDebugDrawer;
    class GameState;

    class Engine: public Ogre::FrameListener
    {
    public:
        Engine();

        Engine(const Engine& p_other) = delete;

        Engine& operator=(const Engine& p_other) = delete;

        ~Engine();

        void pushState(GameState* p_state);

        void mainLoop();

        bool frameStarted(const Ogre::FrameEvent& p_event);

        Ogre::Root* getOgreRoot();

        InputSystem* getInputSystem();

        btDiscreteDynamicsWorld* getDynamicsWorld();

        BulletDebugDrawer* getDebugDrawer();

        void requestPop();

        void requestQuit();

        float getTimeSinceLastFrame();

        GameState* getCurrentState();

    private:
        void popState();

        static constexpr float INPUT_UPDATE_INTERVAL = 0.02;

        // heap object because we have to allocate it within a method
        // and return it from the method
        Ogre::Root* m_ogreRoot;
        Ogre::Root* initOgreRoot();

        btDefaultCollisionConfiguration* m_collisionConfiguration;
        btCollisionDispatcher* m_dispatcher;
        btBroadphaseInterface* m_broadphase;
        btConstraintSolver* m_solver;
        btDiscreteDynamicsWorld* m_dynamicsWorld;
        BulletDebugDrawer* m_debugDrawer;

        InputSystem* m_inputSystem;
        AudioSystem* m_audioSystem;

        CEGUI::OgreRenderer* m_ceguiRenderer;

        float m_timeSinceLastFrame;
        std::vector<GameState*> m_states;

        bool m_requestPop;
        bool m_requestQuit;
    };

    void bulletTickCallback(btDynamicsWorld* p_world, btScalar p_timeStep);
}

#endif
