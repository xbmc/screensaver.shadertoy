cmake_minimum_required(VERSION 3.5)
project(screensaver.shadertoy)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

find_package(Kodi REQUIRED)
find_package(p8-platform REQUIRED)

include_directories(${KODI_INCLUDE_DIR}/.. # Hack way with "/..", need bigger Kodi cmake rework to match right include ways
                    ${p8-platform_INCLUDE_DIRS})

set(SHADERTOY_SOURCES src/toys.cpp)

set(SHADERTOY_HEADERS src/toys.h)

set(DEPLIBS)

build_addon(screensaver.shadertoy SHADERTOY DEPLIBS)

include(CPack)
