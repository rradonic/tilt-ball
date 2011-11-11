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

#include "OgreMotionState.hpp"

namespace TiltBall
{
    OgreMotionState::OgreMotionState(btTransform &p_initialpos, Ogre::SceneNode *p_node)
    {
        m_node = p_node;
        m_position = p_initialpos;
    }

    void OgreMotionState::setNode(Ogre::SceneNode *p_node)
    {
        m_node = p_node;
    }

    void OgreMotionState::getWorldTransform(btTransform &p_worldTrans) const
    {
        p_worldTrans = m_position;
    }

    void OgreMotionState::setWorldTransform(const btTransform &p_worldTrans)
    {
        if(NULL == m_node) return; // silently return before we set a node
        btQuaternion rot = p_worldTrans.getRotation();
        m_node->setOrientation(rot.w(), rot.x(), rot.y(), rot.z());
        btVector3 pos = p_worldTrans.getOrigin();
        m_node->setPosition(pos.x(), pos.y(), pos.z());
    }

    void OgreMotionState::kinematicSetPosition(btTransform &p_worldTrans)
    {
        m_position = p_worldTrans;
    }
}
