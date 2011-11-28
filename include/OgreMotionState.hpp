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

#include <btBulletDynamicsCommon.h>
#include <OGRE/Ogre.h>

namespace TiltBall
{
    class OgreMotionState : public btMotionState
    {
    public:
        OgreMotionState(btTransform& p_initialpos,
                        Ogre::SceneNode* p_node);

        void setNode(Ogre::SceneNode* p_node);

        void getWorldTransform(btTransform& p_worldTrans) const;

        void setWorldTransform(const btTransform& p_worldTrans);

        void kinematicSetPosition(btTransform& p_worldTrans);

    protected:
        Ogre::SceneNode* m_node;
        btTransform m_position;
    };
}
