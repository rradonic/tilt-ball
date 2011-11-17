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

#include "UserData.hpp"

namespace TiltBall
{
    UserData::UserData(Ogre::SceneNode *p_node,
                             Engine *p_engine)
    {
        m_node = p_node;
        m_engine = p_engine;
    }

    Ogre::SceneNode *UserData::getNode()
    {
        return m_node;
    }

    Engine *UserData::getEngine()
    {
        return m_engine;
    }
}
