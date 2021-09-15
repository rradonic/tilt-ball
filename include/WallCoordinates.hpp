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

#ifndef WALLCOORDINATES_HPP
#define	WALLCOORDINATES_HPP

#include <OGRE/Ogre.h>

namespace TiltBall
{
    class WallCoordinates
    {
    public:
        explicit WallCoordinates(int p_beginX, int p_beginZ, int p_endX, int p_endZ);

        int getBeginX();
        int getBeginZ();
        int getEndX();
        int getEndZ();

    private:
        int m_beginX;
        int m_beginZ;
        int m_endX;
        int m_endZ;
    };
}

#endif
