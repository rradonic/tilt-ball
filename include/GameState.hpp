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

#ifndef GAMESTATE_HPP
#define	GAMESTATE_HPP

#include <OGRE/Ogre.h>
#include <OIS/OIS.h>

namespace TiltBall
{
    class Engine;

    class GameState :
        public OIS::MouseListener,
        public OIS::KeyListener
    {
    public:
        explicit GameState(Engine* p_engine);
        virtual ~GameState() = 0;

        virtual void pause() = 0;
        virtual void resume() = 0;
        virtual bool update(const Ogre::FrameEvent& p_event) = 0;

    protected:
        Engine* m_engine;
    };
}

#endif
