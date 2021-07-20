
cmake_minimum_required(VERSION 3.10.2)

project("live")

file(GLOB_RECURSE ALL_SOURCE */*.cpp *.cpp)

include_directories(include render/include egl)

add_library(
        native-lib
        SHARED
        ${ALL_SOURCE})

find_library(
        log-lib
        log )

target_link_libraries(
        native-lib
        EGL
        GLESv2
        android
        ${log-lib} )