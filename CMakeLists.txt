cmake_minimum_required(VERSION 3.5)
project(screensaver.shadertoy)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

find_package(Kodi REQUIRED)
find_package(glm REQUIRED)

if(NOT WIN32 AND (APP_RENDER_SYSTEM STREQUAL "gl" OR NOT APP_RENDER_SYSTEM))
  find_package(OpenGl REQUIRED)
  set(DEPLIBS ${OPENGL_LIBRARIES})
  set(includes ${OPENGL_INCLUDE_DIR})
  add_definitions(${OPENGL_DEFINITIONS})
else()
  find_package(OpenGLES REQUIRED)
  set(DEPLIBS ${OPENGLES_LIBRARIES})
  set(includes ${OPENGLES_INCLUDE_DIR})
  add_definitions(${OPENGLES_DEFINITIONS})
endif()

include_directories(${GLM_INCLUDE_DIR}
                    ${KODI_INCLUDE_DIR}/..) # Hack way with "/..", need bigger Kodi cmake rework to match right include ways

if(CORE_SYSTEM_NAME STREQUAL osx OR
   CORE_SYSTEM_NAME STREQUAL ios OR
   CORE_SYSTEM_NAME STREQUAL darwin_embedded)
  list(APPEND DEPLIBS "-framework CoreVideo")
endif()

set(SHADERTOY_SOURCES src/main.cpp
                      src/lodepng.cpp)

set(SHADERTOY_HEADERS src/main.h
                      src/lodepng.h)

build_addon(screensaver.shadertoy SHADERTOY DEPLIBS)

include(CPack)
