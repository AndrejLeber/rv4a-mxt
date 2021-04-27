cmake_minimum_required(VERSION 3.19)
#set(CMAKE_LEGACY_CYGWIN_WIN32 0)
project(rv4a_mxt)


set(CMAKE_CXX_STANDARD 14)

include_directories(mxt_mvs)

add_executable(rv4a_mxt
        mxt_mvs/main.cpp
        mxt_mvs/r3_protocol.h
        mxt_mvs/strdef.h mxt_mvs/mxt.h mxt_mvs/mxt.cpp mxt_mvs/r3_protocol.cpp)
