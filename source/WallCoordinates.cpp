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

#include "WallCoordinates.hpp"

namespace TiltBall
{
    WallCoordinates::WallCoordinates(int p_beginX, int p_beginZ, int p_endX, int p_endZ):
        m_beginX(p_beginX),
        m_beginZ(p_beginZ),
        m_endX(p_endX),
        m_endZ(p_endZ)
    {
    }

    int WallCoordinates::getBeginX()
    {
        return m_beginX;
    }

    int WallCoordinates::getBeginZ()
    {
        return m_beginZ;
    }

    int WallCoordinates::getEndX()
    {
        return m_endX;
    }

    int WallCoordinates::getEndZ()
    {
        return m_endZ;
    }

}
