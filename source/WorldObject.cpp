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

#include "WorldObject.hpp"

namespace TiltBall
{
    WorldObject::WorldObject(Ogre::MovableObject *p_movableObject,
                             btCollisionShape *p_collisionShape,
                             btTransform p_transform)
    {
        m_movableObject = p_movableObject;
        m_collisionShape = p_collisionShape;
        m_transform = p_transform;
    }

    Ogre::MovableObject *WorldObject::getMovableObject()
    {
        return m_movableObject;
    }

    btCollisionShape *WorldObject::getCollisionShape()
    {
        return m_collisionShape;
    }

    btTransform WorldObject::getTransform()
    {
        return m_transform;
    }
}
