TiltBall
========

TiltBall is a puzzle game in which the goal is to get the ball into
the hole by tilting the maze. Levels are built using a plaintext level
format, making level creation and editing easy.

Installation
------------

TiltBall is being developed on Ubuntu GNU/Linux. It will soon be
packaged in a ppa. Until then, you can build it using the below
instructions.

Building the game should work on other Linux distros as well, just
install the required libraries using your distro's package manager.

Building
--------

The only two required libraries that aren't in the standard repos are the
Bullet physics engine and the CEGUI user interface library. I have
packaged both in a ppa on Launchpad:

https://launchpad.net/~ranko-radonic/+archive/libs

Add this ppa to your apt-get configuration.

    sudo add-apt-repository ppa:ranko-radonic/tilt-ball
    sudo apt-get update

Fetch the game.

    git clone git://github.com/rradonic/tilt-ball.git

Build the game.

    mkdir build
    cd build
    cmake ..
    make

That's it! You can now run the game from inside the build directory
with ./source/tilt-ball

Dependencies
------------

* Bullet (physics)
* Ogre (3D Rendering)
* OIS (input)
* CEGUI (menus etc.)
* Boost::regex (regular expression parsing)
* OpenAL (audio)
* ALUT (audio)
* Vorbis (Ogg Vorbis decoding)
* Vorbisfile (Ogg Vorbis decoding)

TODO: Linux/Windows/MacOSX installation instructions
