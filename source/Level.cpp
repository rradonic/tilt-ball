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
  resources/materials/along with TiltBall.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "Level.hpp"
#include "Engine.hpp"
#include "OgreMotionState.hpp"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <cmath>
#include <vector>
#include <boost/regex.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace TiltBall
{
    Level::Level(Engine* p_engine, std::string p_fileName) :
        m_level(initSceneNode(p_engine, "level")),
        m_ball(initSceneNode(p_engine, "ball")),
        m_target(initSceneNode(p_engine, "target")),
        m_engine(p_engine),
        m_extrusion(0.0003),
        m_fileName(p_fileName)
    {
        load(p_fileName);

        std::clog << "Setting up camera..." << std::endl;
        Ogre::Camera* camera = m_engine->getOgreRoot()->getSceneManager("main_scene_manager")->
            createCamera("main_camera");

        camera->setNearClipDistance(1.0f);
        camera->setFarClipDistance(500.0f);
        camera->setAutoAspectRatio(true);
        camera->setFOVy(Ogre::Degree(70.0f));
        camera->setPosition(m_cameraX, m_cameraY, m_cameraZ);
        camera->lookAt(0, 0, 0);

        std::clog << "Setting up viewport..." << std::endl;
        Ogre::Viewport* viewport = m_engine->getOgreRoot()->getRenderTarget("main_window")->
            addViewport(camera, 0);

        viewport->setBackgroundColour(Ogre::ColourValue(0, 0, 0));

        buildLevel();
        buildBall();
    }

    void Level::buildLevel()
    {
        // bottom surface and walls all go into a compound shape,
        // making the level; the target is a separate shape so we can
        // test for collision separately
        btCompoundShape* compoundShape = new btCompoundShape();

        std::vector<WorldObject> bottomSurface = buildBottomSurface("Materials/Level1Floor");
        for(auto it = bottomSurface.begin(); it < bottomSurface.end(); it++)
        {
            m_level->attachObject((*it).getMovableObject());
            compoundShape->addChildShape((*it).getTransform(),
                                         (*it).getCollisionShape());
        }

        std::vector<WorldObject> walls = buildWalls("Materials/Level1Wall");
        for(auto it = walls.begin(); it < walls.end(); it++)
        {
            m_level->attachObject((*it).getMovableObject());
            compoundShape->addChildShape((*it).getTransform(),
                                         (*it).getCollisionShape());
        }

        WorldObject target = buildBox("target", "Materials/Target",
                                      0 - Level::TARGET_HALF_SIZE,
                                      0 - Level::TARGET_THICKNESS,
                                      0 - Level::TARGET_HALF_SIZE,
                                      Level::TARGET_HALF_SIZE,
                                      Level::TARGET_THICKNESS,
                                      Level::TARGET_HALF_SIZE);

        m_target->attachObject(target.getMovableObject());
        m_target->setPosition(m_levelXMin + m_targetX,
                              m_levelYMin + Level::TARGET_THICKNESS / 2,
                              m_levelZMin + m_targetZ);

        // add level to physics world
        m_collisionShapes.push_back(compoundShape);

        m_levelBody = attachBodyToPhysicsWorld(m_level,
                                               compoundShape,
                                               0,
                                               0,
                                               0,
                                               0);

        // add target to physics world
        m_targetBody = attachBodyToPhysicsWorld(m_target,
                                                target.getCollisionShape(),
                                                0,
                                                m_levelXMin + m_targetX,
                                                m_levelYMin + Level::TARGET_THICKNESS / 2,
                                                m_levelZMin + m_targetZ);

        // add level + target to the graphics world
        Ogre::SceneManager* sceneManager = m_engine->getOgreRoot()->
            getSceneManager("main_scene_manager");
        m_level->addChild(m_target);
        sceneManager->getRootSceneNode()->addChild(m_level);
    }

    void Level::buildBall()
    {
        std::clog << "Creating ball..." << std::endl;
        Ogre::Entity* ball = m_engine->getOgreRoot()->getSceneManager("main_scene_manager")->
            createEntity("ball", "meshes/sphere.mesh");
        ball->setMaterialName("Materials/Ball");

        m_ball->setPosition(m_ballStartingX, 4, m_ballStartingZ);
        m_ball->attachObject(ball);

        // add ball to physics world
        btCollisionShape* sphereShape = new btSphereShape(btScalar(1.0f));
        m_collisionShapes.push_back(sphereShape);

        m_ballBody = attachBodyToPhysicsWorld(m_ball,
                                              sphereShape,
                                              50,
                                              m_ballStartingX,
                                              4,
                                              m_ballStartingZ);

        // add ball to graphics world
        Ogre::SceneManager* sceneManager = m_engine->getOgreRoot()->
            getSceneManager("main_scene_manager");
        sceneManager->getRootSceneNode()->addChild(m_ball);
    }

    Level::~Level()
    {
        m_engine->getOgreRoot()->getSceneManager("main_scene_manager")->clearScene();
        m_engine->getOgreRoot()->getSceneManager("main_scene_manager")->destroyAllCameras();
        m_engine->getOgreRoot()->getRenderTarget("main_window")->removeAllViewports();

        //remove the rigidbodies from the dynamics world and delete them
        btDiscreteDynamicsWorld* dynamicsWorld = m_engine->getDynamicsWorld();
        for(auto i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
        {
            btCollisionObject* object = dynamicsWorld->getCollisionObjectArray()[i];
            btRigidBody* body = btRigidBody::upcast(object);

            if(body)
            {
                if(body->getMotionState())
                    delete body->getMotionState();

                dynamicsWorld->removeRigidBody(body);
            }
            else
                dynamicsWorld->removeCollisionObject(object);

            delete object;
        }

        for(auto it = m_collisionShapes.begin(); it < m_collisionShapes.end(); it++)
            delete (*it);

        m_collisionShapes.clear();
    }

    Ogre::SceneNode* Level::initSceneNode(Engine* p_engine, std::string p_nodeName)
    {
        return p_engine->getOgreRoot()->getSceneManager("main_scene_manager")->
            createSceneNode(p_nodeName);
    }

    std::vector<WorldObject> Level::buildBottomSurface(std::string p_bottomMaterial)
    {
        std::clog << "Creating bottom surface..." << std::endl;

        std::vector<WorldObject> bottomSurface;

        bottomSurface.push_back(buildBox("bottom_surface_1", p_bottomMaterial,
                                         m_levelXMin - Level::WALL_HALF_THICKNESS,
                                         m_levelYMin,
                                         m_levelZMin - Level::WALL_HALF_THICKNESS,
                                         m_levelXMax + Level::WALL_HALF_THICKNESS,
                                         m_levelYMax,
                                         m_levelZMin + m_targetZ - Level::TARGET_HALF_SIZE));

        bottomSurface.push_back(buildBox("bottom_surface_2", p_bottomMaterial,
                                         m_levelXMin + m_targetX + Level::TARGET_HALF_SIZE,
                                         m_levelYMin,
                                         m_levelZMin + m_targetZ - Level::TARGET_HALF_SIZE,
                                         m_levelXMax + Level::WALL_HALF_THICKNESS,
                                         m_levelYMax,
                                         m_levelZMax + Level::WALL_HALF_THICKNESS));

        bottomSurface.push_back(buildBox("bottom_surface_3", p_bottomMaterial,
                                         m_levelXMin - Level::WALL_HALF_THICKNESS,
                                         m_levelYMin,
                                         m_levelZMin + m_targetZ + Level::TARGET_HALF_SIZE,
                                         m_levelXMin + m_targetX + Level::TARGET_HALF_SIZE,
                                         m_levelYMax,
                                         m_levelZMax + Level::WALL_HALF_THICKNESS));

        bottomSurface.push_back(buildBox("bottom_surface_4", p_bottomMaterial,
                                         m_levelXMin - Level::WALL_HALF_THICKNESS,
                                         m_levelYMin,
                                         m_levelZMin + m_targetZ - Level::TARGET_HALF_SIZE,
                                         m_levelXMin + m_targetX - Level::TARGET_HALF_SIZE,
                                         m_levelYMax,
                                         m_levelZMin + m_targetZ + Level::TARGET_HALF_SIZE));

        return bottomSurface;
    }

    std::vector<WorldObject> Level::buildWalls(std::string p_material)
    {
        std::clog << "Creating walls..." << std::endl;

        std::vector<WorldObject> walls;

        int wallNumber = 0;
        for(auto it = m_walls.begin(); it < m_walls.end(); it++, wallNumber++)
        {
            std::ostringstream wallNameStream;
            wallNameStream << "wall" << wallNumber;

            int p_pointBeginX = (*it).getBeginX();
            int p_pointBeginZ = (*it).getBeginZ();
            int p_pointEndX = (*it).getEndX();
            int p_pointEndZ = (*it).getEndZ();

            if (p_pointBeginX > p_pointEndX)
                std::swap(p_pointBeginX, p_pointEndX);
            if (p_pointBeginZ > p_pointEndZ)
                std::swap(p_pointBeginZ, p_pointEndZ);

            float wallX1 = m_levelXMin + p_pointBeginX - Level::WALL_HALF_THICKNESS - m_extrusion;
            float wallX2 = m_levelXMin + p_pointEndX + Level::WALL_HALF_THICKNESS + m_extrusion;
            float wallY1 = m_levelYMax - m_extrusion;
            float wallY2 = m_levelYMax + Level::WALL_HEIGHT + m_extrusion;
            float wallZ1 = m_levelZMin + p_pointBeginZ - Level::WALL_HALF_THICKNESS - m_extrusion;
            float wallZ2 = m_levelZMin + p_pointEndZ + Level::WALL_HALF_THICKNESS + m_extrusion;

            // slight extrusion prevents depth fighting of overlapping wall ends
            m_extrusion += 0.0003;

            walls.push_back(buildBox(wallNameStream.str(),
                                     p_material,
                                     wallX1,
                                     wallY1,
                                     wallZ1,
                                     wallX2,
                                     wallY2,
                                     wallZ2));
        }

        return walls;
    }

    WorldObject Level::buildBox(std::string p_name,
                                std::string p_material,
                                float p_x1,
                                float p_y1,
                                float p_z1,
                                float p_x2,
                                float p_y2,
                                float p_z2)
    {
        Ogre::ManualObject* manual = m_engine->getOgreRoot()->
            getSceneManager("main_scene_manager")->createManualObject(p_name);
        manual->begin(p_material);

        // these two variables are used to avoid texture distortion when the face is not square
        float uMax = 1;
        float vMax = 1;

        // bottom face
        uMax = (p_x2 - p_x1);
        vMax = (p_z2 - p_z1);
        manual->position(p_x1, p_y1, p_z1);
        manual->textureCoord(0, vMax);
        manual->normal(0, -1, 0);

        manual->position(p_x2, p_y1, p_z1);
        manual->textureCoord(uMax, vMax);
        manual->normal(0, -1, 0);

        manual->position(p_x2, p_y1, p_z2);
        manual->textureCoord(uMax, 0);
        manual->normal(0, -1, 0);

        manual->position(p_x1, p_y1, p_z2);
        manual->textureCoord(0, 0);
        manual->normal(0, -1, 0);

        // top face
        manual->position(p_x1, p_y2, p_z2);
        manual->textureCoord(0, 0);
        manual->normal(0, 1, 0);

        manual->position(p_x2, p_y2, p_z2);
        manual->textureCoord(uMax, 0);
        manual->normal(0, 1, 0);

        manual->position(p_x2, p_y2, p_z1);
        manual->textureCoord(uMax, vMax);
        manual->normal(0, 1, 0);

        manual->position(p_x1, p_y2, p_z1);
        manual->textureCoord(0, vMax);
        manual->normal(0, 1, 0);

        // front face
        uMax = (p_x2 - p_x1);
        vMax = (p_y2 - p_y1);

        manual->position(p_x1, p_y1, p_z2);
        manual->textureCoord(0, 0);
        manual->normal(0, 0, 1);

        manual->position(p_x2, p_y1, p_z2);
        manual->textureCoord(uMax, 0);
        manual->normal(0, 0, 1);

        manual->position(p_x2, p_y2, p_z2);
        manual->textureCoord(uMax, vMax);
        manual->normal(0, 0, 1);

        manual->position(p_x1, p_y2, p_z2);
        manual->textureCoord(0, vMax);
        manual->normal(0, 0, 1);

        // back face
        manual->position(p_x1, p_y2, p_z1);
        manual->textureCoord(0, vMax);
        manual->normal(0, 0, -1);

        manual->position(p_x2, p_y2, p_z1);
        manual->textureCoord(uMax, vMax);
        manual->normal(0, 0, -1);

        manual->position(p_x2, p_y1, p_z1);
        manual->textureCoord(uMax, 0);
        manual->normal(0, 0, -1);

        manual->position(p_x1, p_y1, p_z1);
        manual->textureCoord(0, 0);
        manual->normal(0, 0, -1);

        // left face
        uMax = (p_y2 - p_y1);
        vMax = (p_z2 - p_z1);
        manual->position(p_x1, p_y1, p_z2);
        manual->textureCoord(0, 0);
        manual->normal(-1, 0, 0);

        manual->position(p_x1, p_y2, p_z2);
        manual->textureCoord(uMax, 0);
        manual->normal(-1, 0, 0);

        manual->position(p_x1, p_y2, p_z1);
        manual->textureCoord(uMax, vMax);
        manual->normal(-1, 0, 0);

        manual->position(p_x1, p_y1, p_z1);
        manual->textureCoord(0, vMax);
        manual->normal(-1, 0, 0);

        // right face
        manual->position(p_x2, p_y1, p_z1);
        manual->textureCoord(0, vMax);
        manual->normal(1, 0, 0);

        manual->position(p_x2, p_y2, p_z1);
        manual->textureCoord(uMax, vMax);
        manual->normal(1, 0, 0);

        manual->position(p_x2, p_y2, p_z2);
        manual->textureCoord(uMax, 0);
        manual->normal(1, 0, 0);

        manual->position(p_x2, p_y1, p_z2);
        manual->textureCoord(0, 0);
        manual->normal(1, 0, 0);

        // bottom face
        manual->quad(0, 1, 2, 3);
        // top face
        manual->quad(4, 5, 6, 7);
        // front face
        manual->quad(8, 9, 10, 11);
        // back face
        manual->quad(12, 13, 14, 15);
        // left face
        manual->quad(16, 17, 18, 19);
        // right face
        manual->quad(20, 21, 22, 23);

        manual->end();

        btCollisionShape* boxShape = new btBoxShape(btVector3(((p_x2 - p_x1) / 2),
                                                              ((p_y2 - p_y1) / 2),
                                                              ((p_z2 - p_z1) / 2)));

        m_collisionShapes.push_back(boxShape);

        btTransform boxTransform;
        boxTransform.setIdentity();
        boxTransform.setOrigin(btVector3(((p_x2 + p_x1) / 2),
                                         ((p_y2 + p_y1) / 2),
                                         ((p_z2 + p_z1) / 2)));

        return WorldObject(manual, boxShape, boxTransform);
    }

    btRigidBody* Level::attachBodyToPhysicsWorld(Ogre::SceneNode* p_sceneNode,
                                                 btCollisionShape* p_collisionShape,
                                                 float p_mass,
                                                 float p_originX,
                                                 float p_originY,
                                                 float p_originZ)
    {
        btTransform transform;
        transform.setIdentity();
        transform.setOrigin(btVector3(p_originX, p_originY, p_originZ));

        btScalar mass(p_mass);
        btVector3 localInertia(0, 0, 0);
        if(p_mass > 0)
            p_collisionShape->calculateLocalInertia(mass, localInertia);

        OgreMotionState* motionState = new OgreMotionState(transform, p_sceneNode);

        btRigidBody::btRigidBodyConstructionInfo info(mass,
                                                      motionState,
                                                      p_collisionShape,
                                                      localInertia);

        btRigidBody* body = new btRigidBody(info);

        body->setRestitution(0);
        body->setFriction(0.2);

        if(p_mass == 0)
        {
            body->setCollisionFlags(body->getCollisionFlags() |
                                    btCollisionObject::CF_KINEMATIC_OBJECT);
            body->setActivationState(DISABLE_DEACTIVATION);
        }

        body->setUserPointer(p_sceneNode);

        m_engine->getDynamicsWorld()->addRigidBody(body);

        return body;
    }

    void Level::load(std::string p_fileName)
    {
        std::clog << "Loading level..." << std::endl;

        boost::property_tree::ptree pt;
        read_json(p_fileName, pt);

        m_name = pt.get<std::string>("name");

        std::clog << "Level name: " << m_name << std::endl;

        m_levelXMin = -(pt.get<float>("dimensions.x") / 2);
        m_levelXMax = pt.get<float>("dimensions.x") / 2;
        m_levelZMin = -(pt.get<float>("dimensions.z") / 2);
        m_levelZMax = pt.get<float>("dimensions.z") / 2;
        m_levelYMin = -1;
        m_levelYMax = 1;

        std::clog << "Level dimensions: " <<
            pt.get<float>("dimensions.x") << 'x' <<
            pt.get<float>("dimensions.z") << std::endl;

        m_cameraX = pt.get<float>("camera.x");
        m_cameraY = pt.get<float>("camera.y");
        m_cameraZ = pt.get<float>("camera.z");

        m_targetX = pt.get<float>("target.x");
        m_targetZ = pt.get<float>("target.z");

        std::clog << "Target coordinates: " << m_targetX << ' ' << m_targetZ << std::endl;

        m_ballStartingX = pt.get<float>("ball.x");
        m_ballStartingZ = pt.get<float>("ball.z");

        std::clog << "Ball coordinates: " << m_ballStartingX << ' ' << m_ballStartingZ << std::endl;

        std::clog << "Loading walls..." << std::endl;
        for(auto it = pt.get_child("walls").begin(); it != pt.get_child("walls").end(); it++)
            m_walls.push_back(WallCoordinates((*it).second.get<float>("begin.x"),
                                              (*it).second.get<float>("begin.z"),
                                              (*it).second.get<float>("end.x"),
                                              (*it).second.get<float>("end.z")));
    }

    btRigidBody* Level::getLevelBody()
    {
        return m_levelBody;
    }

    btRigidBody* Level::getBallBody()
    {
        return m_ballBody;
    }

    btRigidBody* Level::getTargetBody()
    {
        return m_targetBody;
    }

    Ogre::SceneNode* Level::getLevelNode()
    {
        return m_level;
    }

    Ogre::SceneNode* Level::getBallNode()
    {
        return m_ball;
    }

    Ogre::SceneNode* Level::getTargetNode()
    {
        return m_target;
    }

    std::string Level::getFileName()
    {
        return m_fileName;
    }

    std::string Level::getNextLevelFileName()
    {
        boost::regex regex("(.*)level([[:digit:]]+)\\.json");
        boost::smatch matches;
        regex_search(m_fileName, matches, regex);

        int levelNumber = atoi(matches[2].str().c_str());

        std::stringstream stream;
        stream << matches[1] << "level" << levelNumber + 1 << ".json";

        return stream.str();
    }
}
