project(screensaver.shadertoy)

cmake_minimum_required(VERSION 2.6)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

enable_language(CXX)

find_package(Kodi REQUIRED)
find_package(p8-platform REQUIRED)

include_directories(${KODI_INCLUDE_DIR} ${p8-platform_INCLUDE_DIRS})

set(SHADERTOY_SOURCES src/toys.cpp)

set(DEPLIBS)

build_addon(screensaver.shadertoy SHADERTOY DEPLIBS)

include(CPack)
