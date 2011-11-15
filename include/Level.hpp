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

#ifndef LEVEL_HPP
#define LEVEL_HPP

#include "WallCoordinates.hpp"
#include "WorldObject.hpp"

#include <btBulletDynamicsCommon.h>
#include <OGRE/Ogre.h>

namespace TiltBall
{
    class Engine;

    class Level
    {
    public:
        explicit Level(Engine *p_engine, std::string p_nodeName);

        Level(const Level &p_other) = delete;

        Level &operator=(const Level &p_other) = delete;

        ~Level();

        btRigidBody *getLevelBody();

        btRigidBody *getBallBody();

        Ogre::SceneNode *getLevelNode();

        Ogre::SceneNode *getBallNode();

    protected:
        Ogre::SceneNode *m_level;
        Ogre::SceneNode *m_ball;

    private:
        Ogre::SceneNode *createSceneNode(Engine *p_engine,
                                         std::string p_nodeName);

        void load(std::string p_levelFileName);

        bool getNonEmptyLine(std::ifstream &p_file,
                             std::istringstream &p_lineStream);

        WorldObject buildBox(std::string p_name,
                             std::string p_material,
                             float p_x1,
                             float p_y1,
                             float p_z1,
                             float p_x2,
                             float p_y2,
                             float p_z2);

        std::vector<WorldObject> buildBottomSurface(std::string p_bottomMaterial,
                                                    std::string p_holeMaterial);

        std::vector<WorldObject> buildWalls(std::string p_material);

        void buildLevel();

        void buildBall();

        Engine *m_engine;
        std::string m_name;
        std::vector<WallCoordinates> m_walls;

        static constexpr float WALL_HEIGHT = 2.0;
        static constexpr float WALL_HALF_THICKNESS = 0.6;
        static constexpr float HOLE_HALF_SIZE = 1.3;

        // level dimensions in world coordinates
        float m_levelYMin;
        float m_levelYMax;
        float m_levelXMin;
        float m_levelXMax;
        float m_levelZMin;
        float m_levelZMax;

        float m_holeX;
        float m_holeZ;

        float m_cameraX;
        float m_cameraY;
        float m_cameraZ;

        std::vector<btCollisionShape*> m_collisionShapes;
        btRigidBody *m_levelBody;
        btRigidBody *m_ballBody;
    };
}

#endif
