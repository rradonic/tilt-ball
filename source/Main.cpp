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

#include "Engine.hpp"
#include "IntroState.hpp"
#include "MenuState.hpp"
#include "RunningState.hpp"

#include <fstream>

int main()
{
    try
    {
        std::ofstream log("tilt_ball.log");

        std::streambuf* old = std::clog.rdbuf(log.rdbuf());

        TiltBall::Engine engine;
        engine.pushState(new TiltBall::RunningState(&engine));
        engine.pushState(new TiltBall::MenuState(&engine));
        engine.pushState(new TiltBall::IntroState(&engine));
        engine.mainLoop();

        std::clog.rdbuf(old);
    }
    catch(char const* error)
    {
        std::cerr << "UNCAUGHT EXCEPTION: " << error << std::endl;
    }

    return 0;
}
