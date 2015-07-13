cmake_minimum_required(VERSION 2.8)
project(libgamectrl)

find_package(qibuild)

include_directories(../../include)

set(_srcs
    GameCtrl.cpp
    UdpComm.h
    UdpComm.cpp)

qi_create_lib(libgamectrl SHARED ${_srcs} SUBFOLDER naoqi)

qi_use_lib(libgamectrl ALCOMMON BOOST)