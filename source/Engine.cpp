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

#include "BulletDebugDrawer.hpp"
#include "Engine.hpp"
#include <map>

namespace TiltBall
{
    Engine::Engine() :
        m_ogreRoot(createOgreRoot()),

        m_collisionConfiguration(new btDefaultCollisionConfiguration()),
        m_dispatcher(new btCollisionDispatcher(m_collisionConfiguration)),
        m_broadphase(new btDbvtBroadphase()),
        m_solver(new btSequentialImpulseConstraintSolver()),
        m_dynamicsWorld(new btDiscreteDynamicsWorld(m_dispatcher, m_broadphase, m_solver, m_collisionConfiguration)),

        m_inputSystem(new InputSystem(getOgreRoot())),
        m_ceguiRenderer(&CEGUI::OgreRenderer::bootstrapSystem(*(getOgreRoot()->getRenderTarget("main_window")))),
        m_timeSinceLastFrame(0),
        m_requestPop(false)
    {
        std::clog << "Setting up resource manager..." << std::endl;
        Ogre::ResourceGroupManager* resourceGroupManager = Ogre::ResourceGroupManager::getSingletonPtr();

        resourceGroupManager->addResourceLocation("../resources", "FileSystem", "General", true);

        resourceGroupManager->addResourceLocation("../resources/cegui/imagesets",
                                                  "FileSystem", "Imagesets", true);
        resourceGroupManager->addResourceLocation("/usr/local/share/CEGUI/imagesets",
                                                  "FileSystem", "Imagesets", true);

        resourceGroupManager->addResourceLocation("/usr/local/share/CEGUI/fonts",
                                                  "FileSystem", "Fonts", true);

        resourceGroupManager->addResourceLocation("../resources/cegui/schemes",
                                                  "FileSystem", "Schemes", true);
        resourceGroupManager->addResourceLocation("/usr/local/share/CEGUI/schemes",
                                                  "FileSystem", "Schemes", true);

        resourceGroupManager->addResourceLocation("/usr/local/share/CEGUI/looknfeel",
                                                  "FileSystem", "LookAndFeel", true);

        resourceGroupManager->addResourceLocation("../resources/cegui/layouts",
                                                  "FileSystem", "Layouts", true);
        resourceGroupManager->addResourceLocation("/usr/local/share/CEGUI/layouts",
                                                  "FileSystem", "Layouts", true);

        CEGUI::Imageset::setDefaultResourceGroup("Imagesets");
        CEGUI::Font::setDefaultResourceGroup("Fonts");
        CEGUI::Scheme::setDefaultResourceGroup("Schemes");
        CEGUI::WidgetLookManager::setDefaultResourceGroup("LookAndFeel");
        CEGUI::WindowManager::setDefaultResourceGroup("Layouts");

        resourceGroupManager->initialiseAllResourceGroups();

        CEGUI::SchemeManager::getSingleton().create("TaharezLook.scheme");
        CEGUI::System::getSingleton().setDefaultMouseCursor("TaharezLook", "MouseArrow");

        m_dynamicsWorld->setGravity(btVector3(0,-50,0));

        resourceGroupManager->createResourceGroup("Debugging");

        m_debugDrawer = new BulletDebugDrawer(this);
        m_dynamicsWorld->setDebugDrawer(m_debugDrawer);
    }

    Ogre::Root* Engine::createOgreRoot()
    {
        std::clog << "Initializing Ogre..." << std::endl;
        Ogre::Root* ogreRoot = new Ogre::Root("", "");

        std::clog << "Acquiring rendering system..." << std::endl;
        ogreRoot->loadPlugin("/usr/lib/OGRE/RenderSystem_GL.so");
        Ogre::String name("OpenGL Rendering Subsystem");
        Ogre::RenderSystemList list = ogreRoot->getAvailableRenderers();
        Ogre::RenderSystemList::iterator iter = list.begin();
        Ogre::RenderSystem* renderSystem;

        while (iter != list.end())
        {
            renderSystem = *(iter++);
            if (renderSystem->getName() == name)
            {
                ogreRoot->setRenderSystem(renderSystem);
                break;
            }
        }

        if (ogreRoot->getRenderSystem() == 0)
        {
            std::cerr << "Could not acquire rendering system!" << std::endl;
            exit(EXIT_FAILURE);
        }

        std::clog << "Initializing ogreRoot..." << std::endl;
        ogreRoot->initialise(false);
        ogreRoot->addFrameListener(this);

        std::clog << "Creating render window..." << std::endl;
        Ogre::NameValuePairList params;
        params.insert(std::pair<Ogre::String, Ogre::String>("title", "TiltBall"));
        ogreRoot->createRenderWindow("main_window", 1024, 768, false, &params);

        std::clog << "Creating scene manager..." << std::endl;
        ogreRoot->createSceneManager(Ogre::ST_GENERIC, "main_scene_manager");

        return ogreRoot;
    }

    Engine::~Engine()
    {
        std::clog << "Engine destructor" << std::endl;

        CEGUI::OgreRenderer::destroySystem();
        delete m_inputSystem;

        delete m_dynamicsWorld;
        delete m_solver;
        delete m_broadphase;
        delete m_dispatcher;
        delete m_collisionConfiguration;
        delete m_debugDrawer;

        delete m_ogreRoot;
    }

    void Engine::pushState(GameState* p_state)
    {
        m_inputSystem->getKeyboard()->setEventCallback(0);
        m_inputSystem->getMouse()->setEventCallback(0);

        m_states.push_back(p_state);

        m_inputSystem->getKeyboard()->setEventCallback(m_states.back());
        m_inputSystem->getMouse()->setEventCallback(m_states.back());
    }

    void Engine::popState()
    {
        m_inputSystem->getKeyboard()->setEventCallback(0);
        m_inputSystem->getMouse()->setEventCallback(0);

        GameState* oldState = m_states.back();
        m_states.pop_back();

        m_inputSystem->getKeyboard()->setEventCallback(m_states.back());
        m_inputSystem->getMouse()->setEventCallback(m_states.back());

        delete oldState;
    }

    void Engine::requestPop()
    {
        m_requestPop = true;
    }

    void Engine::mainLoop()
    {
        std::clog << "Entering main loop..." << std::endl;

        bool keepRendering = true;
        while(keepRendering)
        {
            if(m_requestPop)
            {
                popState();
                m_requestPop = false;
            }
            if(m_states.size() == 0)
                break;

            Ogre::WindowEventUtilities::messagePump();
            keepRendering = getOgreRoot()->renderOneFrame();
        }
    }

    Ogre::Root* Engine::getOgreRoot()
    {
        return m_ogreRoot;
    }

    BulletDebugDrawer* Engine::getDebugDrawer()
    {
        return m_debugDrawer;
    }

    btDiscreteDynamicsWorld* Engine::getDynamicsWorld()
    {
        return m_dynamicsWorld;
    }

    bool Engine::frameStarted(const Ogre::FrameEvent& p_event)
    {
        m_timeSinceLastFrame += p_event.timeSinceLastFrame;
        if (m_timeSinceLastFrame < INPUT_UPDATE_INTERVAL)
            return true;

        bool ret = m_states.back()->update(p_event);

        m_timeSinceLastFrame = 0;

        return ret;
    }

    InputSystem* Engine::getInputSystem()
    {
        return m_inputSystem;
    }

    float Engine::getTimeSinceLastFrame()
    {
        return m_timeSinceLastFrame;
    }
}