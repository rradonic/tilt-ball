TiltBall
========

TiltBall is a puzzle game in which the goal is to get the ball into
the hole by tilting the maze. Levels are built using a plaintext level
format, making level creation and editing easy.

Demo video
----------

[![Tilt Ball demo video](http://img.youtube.com/vi/e5HwLi-JR-w/0.jpg)](http://www.youtube.com/watch?v=e5HwLi-JR-w "Tilt Ball demo video")

Installation
------------

TiltBall is being developed on Ubuntu GNU/Linux. It will soon be
packaged in a ppa. Until then, you can build it using the below
instructions.

Building the game should work on other Linux distros as well, just
install the required libraries using your distro's package manager.

Building
--------

1) Install the libaries that the game needs.

    sudo apt-get install cmake libbullet-dev libogre-1.8-dev libois-dev libcegui-mk2-dev libboost-regex-dev libopenal-dev libalut-dev libvorbis-dev libvorbisfile3

2) Fetch the game.

    git clone git://github.com/rradonic/tilt-ball.git

3) Build the game.

    mkdir build
    cd build
    cmake ..
    make

That's it! You can now run the game from inside the build directory
with ./source/tilt-ball

Dependencies
------------

* CMake (build system)
* Bullet (physics)
* Ogre (3D Rendering)
* OIS (input)
* CEGUI (menus etc.)
* Boost::regex (regular expression parsing)
* OpenAL (audio)
* ALUT (audio)
* Vorbis (Ogg Vorbis decoding)
* Vorbisfile (Ogg Vorbis decoding)
