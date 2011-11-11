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

#ifndef BULLETDEBUGDRAWER_HPP
#define BULLETDEBUGDRAWER_HPP

#include <LinearMath/btIDebugDraw.h>
#include <OGRE/Ogre.h>
#include <vector>

namespace TiltBall
{
    class Engine;

    class BulletDebugDrawer : public btIDebugDraw
    {
    public:
        explicit BulletDebugDrawer(Engine* p_engine);

        void drawLine(const btVector3 &p_from, const btVector3 &p_to, const btVector3 &p_color);
        void drawContactPoint(const btVector3& p_pointOnB, const btVector3& p_normalOnB,
                              btScalar p_distance, int p_lifeTime, const btVector3& p_color);
        void reportErrorWarning(const char* p_warningString);
        void draw3dText(const btVector3& p_location, const char* p_textString);
        void setDebugMode(int p_debugMode);
        int getDebugMode() const;

        void clear();
    private:
        Engine* m_engine;
        Ogre::Material* m_material;
        std::vector<Ogre::SceneNode*> m_lines;
    };
}
#endif
