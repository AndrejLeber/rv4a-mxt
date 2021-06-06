cmake_minimum_required(VERSION 3.19)
#set(CMAKE_LEGACY_CYGWIN_WIN32 0)
project(rv4a_mxt)


set(CMAKE_CXX_STANDARD 17)

include_directories(mxt_mvs)

add_executable(
        rv4a_mxt
        mxt_mvs/main.cpp
        mxt_mvs/r3_protocol.h
        mxt_mvs/r3_protocol.cpp
        mxt_mvs/strdef.h
        mxt_mvs/mxt.h
        mxt_mvs/mxt.cpp
        mxt_mvs/Bahnplanung.h
        mxt_mvs/Bahnplanung.cpp
        mxt_mvs/rt.h
        mxt_mvs/rt.cpp
        mxt_mvs/GCode.h
        mxt_mvs/GCode.cpp
        )
