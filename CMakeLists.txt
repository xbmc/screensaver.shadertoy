project(screensaver.shadertoy)

cmake_minimum_required(VERSION 3.3)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

enable_language(CXX)

find_package(Kodi REQUIRED)

if(NOT WIN32)
  if(APP_RENDER_SYSTEM STREQUAL "gl" OR NOT APP_RENDER_SYSTEM)
    find_package(OpenGl REQUIRED)
    set(DEPLIBS ${OPENGL_LIBRARIES})
    set(includes ${OPENGL_INCLUDE_DIR})
    add_definitions(${OPENGL_DEFINITIONS})
  elseif(APP_RENDER_SYSTEM STREQUAL "gles")
    find_package(OpenGLES REQUIRED)
    set(DEPLIBS ${OPENGLES_LIBRARIES})
    set(includes ${OPENGLES_INCLUDE_DIR})
    add_definitions(${OPENGLES_DEFINITIONS})
  endif()

  if(NOT OPENGL_FOUND AND NOT OPENGLES_FOUND)
    message(FATAL_ERROR "No rendering API found. Bailing")
  endif()

  find_package(glm REQUIRED)
  list(APPEND DEPENDS glm)
  include_directories(${GLM_INCLUDE_DIRS})

  if(APPLE)
    list(APPEND DEPLIBS "-framework CoreVideo")
  endif()
endif()

include_directories(${includes}
                    ${KODI_INCLUDE_DIR}/..  # Hack way with "/..", need bigger Kodi cmake rework to match right include ways
                    ${PROJECT_SOURCE_DIR}/lib)

if(WIN32)
  set(APP_RENDER_SYSTEM dx11)
  set(SHADERTOY_SOURCES src/toysDX.cpp
                        src/WICTextureLoader.cpp)
  set(SHADERTOY_HEADERS src/toys.h
                        src/WICTextureLoader.h)
else()
  set(SHADERTOY_SOURCES src/toysGL.cpp
                        src/lodepng.cpp)
  set(SHADERTOY_HEADERS src/toys.h
                        src/lodepng.h)

endif()

message(STATUS "Configured render system: ${APP_RENDER_SYSTEM}")

build_addon(screensaver.shadertoy SHADERTOY DEPLIBS)

include(CPack)
