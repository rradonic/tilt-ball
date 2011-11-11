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

#include "BulletDebugDrawer.hpp"
#include "Engine.hpp"

#include <sstream>

namespace TiltBall
{
    BulletDebugDrawer::BulletDebugDrawer(Engine* p_engine) :
        m_engine(p_engine),
        m_material(dynamic_cast<Ogre::Material*>(Ogre::MaterialManager::getSingletonPtr()->create("Debug/BulletDebugDrawMaterial","Debugging").get()))
    {
        m_material->setReceiveShadows(false);
        m_material->getTechnique(0)->setLightingEnabled(true);
        m_material->getTechnique(0)->getPass(0)->setDiffuse(0,0,1,0);
        m_material->getTechnique(0)->getPass(0)->setAmbient(0,0,1);
        m_material->getTechnique(0)->getPass(0)->setSelfIllumination(0,0,1);
    }

    void BulletDebugDrawer::drawLine(const btVector3 &p_from, const btVector3 &p_to, const btVector3 &p_color)
    {
        Ogre::SceneManager* sceneManager = m_engine->getOgreRoot()->getSceneManager("main_scene_manager");

        std::ostringstream lineNodeName;
        lineNodeName << "lineNode" << m_lines.size();
        std::ostringstream lineObjectName;
        lineObjectName << "lineObject" << m_lines.size();
        Ogre::SceneNode* manualObjectNode = sceneManager->getRootSceneNode()->createChildSceneNode(lineNodeName.str());
        Ogre::ManualObject* manualObject = sceneManager->createManualObject(lineObjectName.str());

        manualObject->begin("Debug/BulletDebugMaterial", Ogre::RenderOperation::OT_LINE_LIST);
        manualObject->position(p_from.getX(), p_from.getY(), p_from.getZ());
        manualObject->position(p_to.getX(), p_to.getY(), p_to.getZ());
        manualObject->end();

        manualObjectNode->attachObject(manualObject);
        m_lines.push_back(manualObjectNode);
    }

    void BulletDebugDrawer::clear()
    {
        Ogre::SceneManager* sceneManager = m_engine->getOgreRoot()->getSceneManager("main_scene_manager");

        std::vector<Ogre::SceneNode*>::iterator iter;
        for(iter = m_lines.begin(); iter < m_lines.end(); iter++)
        {
            Ogre::ManualObject* manualObject = dynamic_cast<Ogre::ManualObject*>((*iter)->getAttachedObject(0));
            std::clog << std::string("Destroying line object ") + manualObject->getName() << std::endl;
            (*iter)->detachObject(manualObject);
            sceneManager->destroyManualObject(manualObject);
            std::clog << std::string("Destroying line node ") + (*iter)->getName() << std::endl;
            sceneManager->getRootSceneNode()->removeAndDestroyChild((*iter)->getName());
        }
        m_lines.clear();
    }

    void BulletDebugDrawer::drawContactPoint(const btVector3& p_pointOnB, const btVector3& p_normalOnB,
                                             btScalar p_distance, int p_lifeTime, const btVector3& p_color)
    {
    }

    void BulletDebugDrawer::reportErrorWarning(const char* p_warningString)
    {
    }

    void BulletDebugDrawer::draw3dText(const btVector3& p_location, const char* p_textString)
    {
    }

    void BulletDebugDrawer::setDebugMode(int p_debugMode)
    {
    }

    int BulletDebugDrawer::getDebugMode() const
    {
        return btIDebugDraw::DBG_DrawWireframe;
    }
}
