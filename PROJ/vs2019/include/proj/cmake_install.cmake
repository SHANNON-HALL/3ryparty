# Install script for directory: F:/code/3rdparty/PROJ/PROJ-master/include/proj

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "F:/code/3rdparty/PROJ/install")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/proj" TYPE FILE FILES
    "F:/code/3rdparty/PROJ/PROJ-master/include/proj/util.hpp"
    "F:/code/3rdparty/PROJ/PROJ-master/include/proj/metadata.hpp"
    "F:/code/3rdparty/PROJ/PROJ-master/include/proj/common.hpp"
    "F:/code/3rdparty/PROJ/PROJ-master/include/proj/crs.hpp"
    "F:/code/3rdparty/PROJ/PROJ-master/include/proj/datum.hpp"
    "F:/code/3rdparty/PROJ/PROJ-master/include/proj/coordinatesystem.hpp"
    "F:/code/3rdparty/PROJ/PROJ-master/include/proj/coordinateoperation.hpp"
    "F:/code/3rdparty/PROJ/PROJ-master/include/proj/io.hpp"
    "F:/code/3rdparty/PROJ/PROJ-master/include/proj/nn.hpp"
    )
endif()

