cmake_minimum_required(VERSION 3.0.0)
project(monsys VERSION 0.1.0 LANGUAGES C CXX)

include_directories(/usr/include)
include_directories(/usr/local/include)

include_directories(/usr/include/postgresql)
include_directories(/usr/local/include/net-snmp/library)

add_executable(monsys monsys.cc)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

include(CPack)

add_library ( lib-mon STATIC
    AlarmBoolRobot.cc
    Dispatcher.cc
    Logger.cc
    LogBackend.cc
    Config.cc
    Expression.cc
    Filtered.cc
    MTSync.cc
    Network.cc
    Message.cc
    Message_Body.cc
    MessageQueue.cc
    MessagePool.cc
    MessageBuffer.cc
    Parser.cc
    Parser.tab.cc
    PGBackend.cc
    PseudoNetwork.cc
    Resource.cc
    Runable.cc
    Robot.cc
    RndSensor.cc
    SNMPSensor.cc
    SNMPNetwork.cc
    Sensor.cc
    S2SMapRobot.cc
    Parameter.cc
    utils.cc
    )

#add_library(lib_snmp STATIC IMPORTED)
#set_property(TARGET lib_snmp PROPERTY
#                 IMPORTED_LOCATION /usr/local/lib/libnetsnmp.a)

#add_library(lib_psql STATIC IMPORTED)
#set_property(TARGET lib_psql PROPERTY
#                                  IMPORTED_LOCATION /usr/lib/x86_64-linux-gnu/libpq.a)


#target_compile_definitions(lib-mon PUBLIC EXPRESSIONS )
#target_compile_definitions(monsys  PUBLIC EXPRESSIONS )

#target_compile_definitions(lib-mon PUBLIC EXPRESSIONS NETSNMP)
#target_compile_definitions(monsys  PUBLIC EXPRESSIONS NETSNMP)

target_compile_definitions(lib-mon PUBLIC EXPRESSIONS PSQL NETSNMP)
target_compile_definitions(monsys  PUBLIC EXPRESSIONS PSQL NETSNMP)


target_compile_options(lib-mon PUBLIC -pthread -fPIC -Wno-deprecated)
target_compile_options(monsys PUBLIC -pthread -fPIC -Wno-deprecated)

target_compile_features(monsys PUBLIC cxx_std_14)
target_compile_features(lib-mon PUBLIC cxx_std_14)


target_link_libraries(monsys
lib-mon
#lib_snmp
#lib_psql
-lpthread
/usr/lib/x86_64-linux-gnu/libpq.so
/usr/local/lib/libnetsnmp.so
)

