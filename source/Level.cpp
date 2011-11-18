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
#include "UserData.hpp"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <cmath>
#include <vector>

namespace TiltBall
{
    Level::Level(Engine *p_engine, std::string p_levelFileName) :
        m_level(createSceneNode(p_engine, "level")),
        m_ball(createSceneNode(p_engine, "ball")),
        m_target(createSceneNode(p_engine, "target")),
        m_engine(p_engine)
    {
        load(p_levelFileName);

        std::clog << "Setting up camera..." << std::endl;
        Ogre::Camera *camera = m_engine->getOgreRoot()->getSceneManager("main_scene_manager")->
            createCamera("main_camera");

        camera->setNearClipDistance(1.0f);
        camera->setFarClipDistance(500.0f);
        camera->setAutoAspectRatio(true);
        camera->setFOVy(Ogre::Degree(70.0f));
        camera->setPosition(m_cameraX, m_cameraY, m_cameraZ);
        camera->lookAt(0, 0, 0);

        std::clog << "Setting up viewport..." << std::endl;
        Ogre::Viewport *viewport = m_engine->getOgreRoot()->getRenderTarget("main_window")->
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
        btCompoundShape *compoundShape = new btCompoundShape();

        std::vector<WorldObject> bottomSurface = buildBottomSurface("Materials/Level1Floor");
        for(std::vector<WorldObject>::iterator it = bottomSurface.begin();
            it < bottomSurface.end();
            it++)
        {
            m_level->attachObject((*it).getMovableObject());
            compoundShape->addChildShape((*it).getTransform(),
                                         (*it).getCollisionShape());
        }

        std::vector<WorldObject> walls = buildWalls("Materials/Level1Wall");
        for(std::vector<WorldObject>::iterator it = walls.begin();
            it < walls.end();
            it++)
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

        btTransform levelTransform;
        levelTransform.setIdentity();
        levelTransform.setOrigin(btVector3(0, 0, 0));

        btScalar levelMass(0.f);
        btVector3 levelLocalInertia(0, 0, 0);

        OgreMotionState *levelMotionState = new OgreMotionState(levelTransform, m_level);

        btRigidBody::btRigidBodyConstructionInfo levelInfo(levelMass,
                                                           levelMotionState,
                                                           compoundShape,
                                                           levelLocalInertia);

        m_levelBody = new btRigidBody(levelInfo);
        m_levelBody->setCollisionFlags(m_levelBody->getCollisionFlags() |
                                       btCollisionObject::CF_KINEMATIC_OBJECT);
        m_levelBody->setActivationState(DISABLE_DEACTIVATION);
        m_levelBody->setUserPointer(new UserData(m_level, m_engine));

        m_engine->getDynamicsWorld()->addRigidBody(m_levelBody);

        // add target to physics world
        btTransform targetTransform;
        targetTransform.setIdentity();
        targetTransform.setOrigin(btVector3(m_levelXMin + m_targetX,
                                            m_levelYMin + Level::TARGET_THICKNESS / 2,
                                            m_levelZMin + m_targetZ));

        btScalar targetMass(0.f);
        btVector3 targetLocalInertia(0, 0, 0);

        OgreMotionState *targetMotionState = new OgreMotionState(targetTransform, m_target);

        btRigidBody::btRigidBodyConstructionInfo targetInfo(targetMass,
                                                            targetMotionState,
                                                            target.getCollisionShape(),
                                                            targetLocalInertia);

        m_targetBody = new btRigidBody(targetInfo);
        m_targetBody->setCollisionFlags(m_targetBody->getCollisionFlags() |
                                        btCollisionObject::CF_KINEMATIC_OBJECT);
        m_targetBody->setActivationState(DISABLE_DEACTIVATION);
        m_targetBody->setUserPointer(new UserData(m_target, m_engine));

        m_engine->getDynamicsWorld()->addRigidBody(m_targetBody);

        // add level + target to the graphics world
        Ogre::SceneManager *sceneManager = m_engine->getOgreRoot()->
            getSceneManager("main_scene_manager");
        m_level->addChild(m_target);
        sceneManager->getRootSceneNode()->addChild(m_level);
    }

    void Level::buildBall()
    {
        std::clog << "Creating ball..." << std::endl;
        Ogre::Entity *ball = m_engine->getOgreRoot()->getSceneManager("main_scene_manager")->
            createEntity("ball", "meshes/sphere.mesh");
        ball->setMaterialName("Materials/Ball");

        m_ball->setPosition(-2, 4, -2);
        m_ball->attachObject(ball);

        // add ball to physics world
        btCollisionShape *sphereShape = new btSphereShape(btScalar(1.0f));
        m_collisionShapes.push_back(sphereShape);

        btTransform sphereTransform;
        sphereTransform.setIdentity();
        sphereTransform.setOrigin(btVector3(-2, 2, -2));

        btScalar sphereMass(50.f);
        btVector3 sphereLocalInertia(0, 0, 0);
        sphereShape->calculateLocalInertia(sphereMass, sphereLocalInertia);

        OgreMotionState *sphereMotionState = new OgreMotionState(sphereTransform, m_ball);

        btRigidBody::btRigidBodyConstructionInfo sphereInfo(sphereMass,
                                                            sphereMotionState,
                                                            sphereShape,
                                                            sphereLocalInertia);

        m_ballBody = new btRigidBody(sphereInfo);
        m_ballBody->setUserPointer(new UserData(m_ball, m_engine));

        m_engine->getDynamicsWorld()->addRigidBody(m_ballBody);

        // add ball to graphics world
        Ogre::SceneManager *sceneManager = m_engine->getOgreRoot()->
            getSceneManager("main_scene_manager");
        sceneManager->getRootSceneNode()->addChild(m_ball);
    }

    Level::~Level()
    {
        m_engine->getOgreRoot()->getSceneManager("main_scene_manager")->clearScene();
        m_engine->getOgreRoot()->getSceneManager("main_scene_manager")->destroyAllCameras();
        m_engine->getOgreRoot()->getRenderTarget("main_window")->removeAllViewports();

        //remove the rigidbodies from the dynamics world and delete them
        btDiscreteDynamicsWorld *dynamicsWorld = m_engine->getDynamicsWorld();
        for(int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
        {
            btCollisionObject *object = dynamicsWorld->getCollisionObjectArray()[i];
            btRigidBody *body = btRigidBody::upcast(object);

            if(body)
            {
                if(body->getMotionState())
                    delete body->getMotionState();

                dynamicsWorld->removeRigidBody(body);
            }
            else
                dynamicsWorld->removeCollisionObject(object);

            UserData *data = static_cast<UserData*>(object->getUserPointer());
            if(data)
                delete data;

            delete object;
        }

        for(std::vector<btCollisionShape*>::iterator it = m_collisionShapes.begin();
            it < m_collisionShapes.end();
            it++)
        {
            delete (*it);
        }

        m_collisionShapes.clear();
    }

    Ogre::SceneNode *Level::createSceneNode(Engine *p_engine, std::string p_nodeName)
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

        std::vector<WallCoordinates>::iterator it;
        int wallNumber;
        for(it = m_walls.begin(), wallNumber = 0; it < m_walls.end(); it++, wallNumber++)
        {
            std::ostringstream wallNameStream;
            wallNameStream << "wall" << wallNumber;

            int p_pointBeginX = (*it).getBeginX();
            int p_pointBeginZ = (*it).getBeginZ();
            int p_pointEndX = (*it).getEndX();
            int p_pointEndZ = (*it).getEndZ();

            static float extrusion = 0.001;
            if (p_pointBeginX > p_pointEndX)
                std::swap(p_pointBeginX, p_pointEndX);
            if (p_pointBeginZ > p_pointEndZ)
                std::swap(p_pointBeginZ, p_pointEndZ);

            float wallX1 = m_levelXMin + p_pointBeginX - Level::WALL_HALF_THICKNESS - extrusion;
            float wallX2 = m_levelXMin + p_pointEndX + Level::WALL_HALF_THICKNESS + extrusion;
            float wallY1 = m_levelYMax - extrusion;
            float wallY2 = m_levelYMax + Level::WALL_HEIGHT + extrusion;
            float wallZ1 = m_levelZMin + p_pointBeginZ - Level::WALL_HALF_THICKNESS - extrusion;
            float wallZ2 = m_levelZMin + p_pointEndZ + Level::WALL_HALF_THICKNESS + extrusion;

            // slight extrusion prevents depth fighting of overlapping wall ends
            extrusion += 0.001;

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
        Ogre::ManualObject *manual = m_engine->getOgreRoot()->
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

        // add box to the physics world
        btCollisionShape *boxShape = new btBoxShape(btVector3(((p_x2 - p_x1) / 2),
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

    void Level::load(std::string p_levelFileName)
    {
        std::clog << "Loading level..." << std::endl;

        std::ifstream file(p_levelFileName.c_str());
        std::istringstream lineStream;
        std::string throwAway;

        getNonEmptyLine(file, lineStream);
        std::getline(lineStream, throwAway, ' '); lineStream >> m_name;

        std::clog << "Level name:" << std::endl;
        std::clog << m_name << std::endl;

        getNonEmptyLine(file, lineStream);
        float levelXSpan, levelZSpan;
        std::getline(lineStream, throwAway, ' '); lineStream >> levelXSpan;
        std::getline(lineStream, throwAway, 'x'); lineStream >> levelZSpan;

        m_levelXMin = -(levelXSpan / 2);
        m_levelXMax = levelXSpan / 2;
        m_levelZMin = -(levelZSpan / 2);
        m_levelZMax = levelZSpan / 2;
        m_levelYMin = -1;
        m_levelYMax = 1;

        std::clog << "Level dimensions:" << levelXSpan << 'x' << levelZSpan << std::endl;

        getNonEmptyLine(file, lineStream);
        std::getline(lineStream, throwAway, '('); lineStream >> m_cameraX;
        std::getline(lineStream, throwAway, ','); lineStream >> m_cameraY;
        std::getline(lineStream, throwAway, ','); lineStream >> m_cameraZ;

        getNonEmptyLine(file, lineStream);
        std::getline(lineStream, throwAway, '('); lineStream >> m_targetX;
        std::getline(lineStream, throwAway, ','); lineStream >> m_targetZ;

        std::clog << "Target coordinates: " << m_targetX << ' ' << m_targetZ << std::endl;

        std::clog << "Loading walls..." << std::endl;
        // the rest of the file defines the walls
        while (true)
        {
            if (!getNonEmptyLine(file, lineStream))
                break;

            int wallBeginX, wallBeginZ, wallEndX, wallEndZ;

            std::getline(lineStream, throwAway, '('); lineStream >> wallBeginX;
            std::getline(lineStream, throwAway, ','); lineStream >> wallBeginZ;
            std::getline(lineStream, throwAway, '('); lineStream >> wallEndX;
            std::getline(lineStream, throwAway, ','); lineStream >> wallEndZ;

            std::clog << "Wall loaded:" << std::endl;
            std::clog << wallBeginX << std::endl;
            std::clog << wallBeginZ << std::endl;
            std::clog << wallEndX << std::endl;
            std::clog << wallEndZ << std::endl;

            m_walls.push_back(WallCoordinates(wallBeginX, wallBeginZ, wallEndX, wallEndZ));
        }
    }

    bool Level::getNonEmptyLine(std::ifstream &p_file, std::istringstream &p_lineStream)
    {
        std::string line;
        // skip blank lines
        do
        {
            std::getline(p_file, line);
            if (p_file.eof())
                return false;
        } while (line.length() == 0);

        p_lineStream.clear();
        p_lineStream.str(line);

        return true;
    }

    btRigidBody *Level::getLevelBody()
    {
        return m_levelBody;
    }

    btRigidBody *Level::getBallBody()
    {
        return m_ballBody;
    }

    btRigidBody *Level::getTargetBody()
    {
        return m_targetBody;
    }

    Ogre::SceneNode *Level::getLevelNode()
    {
        return m_level;
    }

    Ogre::SceneNode *Level::getBallNode()
    {
        return m_ball;
    }

    Ogre::SceneNode *Level::getTargetNode()
    {
        return m_target;
    }
}
