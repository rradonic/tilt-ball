include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(/usr/include/OGRE)
include_directories(/usr/include/bullet)
include_directories(/usr/include/cegui-0.8.4)

add_executable(tilt-ball
  AudioSystem.cpp
  BulletDebugDrawer.cpp
  Engine.cpp
  GameState.cpp
  InputSystem.cpp
  IntroState.cpp
  Main.cpp
  MenuState.cpp
  OgreMotionState.cpp
  RunningState.cpp
  Level.cpp
  WallCoordinates.cpp
  WorldObject.cpp)

target_link_libraries(tilt-ball
  LinearMath
  BulletCollision
  BulletDynamics
  BulletSoftBody
  OgreMain
  OIS
  CEGUIBase
  CEGUIOgreRenderer
  boost_regex
  boost_system
  openal
  alut
  vorbis
  vorbisfile)

install(TARGETS tilt-ball
  RUNTIME DESTINATION bin)
