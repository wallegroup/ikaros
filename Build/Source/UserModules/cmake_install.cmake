# Install script for directory: /home/olof/ikaros/Source/UserModules

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "/usr/local")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)
STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
IF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  IF(BUILD_TYPE)
    STRING(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  ELSE(BUILD_TYPE)
    SET(CMAKE_INSTALL_CONFIG_NAME "")
  ENDIF(BUILD_TYPE)
  MESSAGE(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
ENDIF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)

# Set the component getting installed.
IF(NOT CMAKE_INSTALL_COMPONENT)
  IF(COMPONENT)
    MESSAGE(STATUS "Install component: \"${COMPONENT}\"")
    SET(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  ELSE(COMPONENT)
    SET(CMAKE_INSTALL_COMPONENT)
  ENDIF(COMPONENT)
ENDIF(NOT CMAKE_INSTALL_COMPONENT)

# Install shared libraries without execute permission?
IF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  SET(CMAKE_INSTALL_SO_NO_EXE "1")
ENDIF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)

IF(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  INCLUDE("/home/olof/ikaros/Build/Source/UserModules/MinimalModule/cmake_install.cmake")
  INCLUDE("/home/olof/ikaros/Build/Source/UserModules/MyModule/cmake_install.cmake")
  INCLUDE("/home/olof/ikaros/Build/Source/UserModules/FaceLocation/cmake_install.cmake")
  INCLUDE("/home/olof/ikaros/Build/Source/UserModules/LogicModelModule/cmake_install.cmake")
  INCLUDE("/home/olof/ikaros/Build/Source/UserModules/MarkToWorld/cmake_install.cmake")
  INCLUDE("/home/olof/ikaros/Build/Source/UserModules/AverageMatrixValue/cmake_install.cmake")
  INCLUDE("/home/olof/ikaros/Build/Source/UserModules/FocusSelector/cmake_install.cmake")
  INCLUDE("/home/olof/ikaros/Build/Source/UserModules/DepthSeg/cmake_install.cmake")
  INCLUDE("/home/olof/ikaros/Build/Source/UserModules/BuildingArea/cmake_install.cmake")
  INCLUDE("/home/olof/ikaros/Build/Source/UserModules/SummedBoxes/cmake_install.cmake")
  INCLUDE("/home/olof/ikaros/Build/Source/UserModules/DirectionOfChange/cmake_install.cmake")
  INCLUDE("/home/olof/ikaros/Build/Source/UserModules/ConstantOutput/cmake_install.cmake")
  INCLUDE("/home/olof/ikaros/Build/Source/UserModules/FocusSelector2/cmake_install.cmake")
  INCLUDE("/home/olof/ikaros/Build/Source/UserModules/FocusSelector3/cmake_install.cmake")
  INCLUDE("/home/olof/ikaros/Build/Source/UserModules/SmoothMovement/cmake_install.cmake")
  INCLUDE("/home/olof/ikaros/Build/Source/UserModules/FocusSelectorNoFace/cmake_install.cmake")

ENDIF(NOT CMAKE_INSTALL_LOCAL_ONLY)

