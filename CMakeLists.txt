cmake_minimum_required(VERSION 3.0.0)
project(monsys VERSION 0.1.0 LANGUAGES C CXX)

include_directories(/usr/include)
include_directories(/usr/local/include)

add_executable(MonSys monsys.cc)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

add_library ( lib-mon STATIC
    
    Dispatcher.cc
    Logger.cc
    LogBackend.cc
    Config.cc
    Filtered.cc
    MTSync.cc
    Network.cc
    Message.cc
    Message_Body.cc
    MessageQueue.cc
    MessagePool.cc
    MessageBuffer.cc
    PseudoNetwork.cc    
    Resource.cc
    Runable.cc
    Robot.cc
    RndSensor.cc
    Sensor.cc
    S2SMapRobot.cc
    Parameter.cc
    utils.cc
)

target_compile_options(lib-mon
  PUBLIC
    -pthread -fPIC -Wno-deprecated
      )

target_compile_options(MonSys
  PUBLIC
    -pthread -fPIC -Wno-deprecated
)

target_link_libraries(MonSys lib-mon -lpthread)

