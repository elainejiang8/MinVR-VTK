# CMakeLists files in this project can
# refer to the root source directory of the project as ${HELLO_SOURCE_DIR} and
# to the root binary directory of the project as ${HELLO_BINARY_DIR}.
cmake_minimum_required (VERSION 2.8.2)

project (MinVR-VTK)

make_directory(${CMAKE_BINARY_DIR}/bin)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# This is the path where cmake will look for files like "FindGLEW.cmake".
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

# If you don't set one of these build types on the command line, with
# 'cmake -DCMAKE_BUILD_TYPE=Debug' or something, the default is 'Debug'.
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE 
    Debug CACHE STRING
    "The build options are: None Debug Release RelWithDebInfo MinSizeRel." 
    FORCE)
endif()

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

find_package(OpenGL REQUIRED)
message("-- OpenGL includes:  " ${OPENGL_INCLUDE_DIR})
message("-- OpenGL library:   " ${OPENGL_LIBRARY})

set(GLM_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/external/glm-0.9.7.1)
message("-- GLM includes:     " ${GLM_INCLUDE_DIR})

find_package(GLEW REQUIRED)
message("-- GLEW library:     " ${GLEW_LIBRARY})
message("-- GLEW includes:    " ${GLEW_INCLUDE_DIRS})

set(GLM_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/external/glm-0.9.7.1)
message("-- GLM includes:     " ${GLM_INCLUDE_DIR})

find_package(FreeGLUT REQUIRED)
message("-- FreeGLUT includes:" ${FREEGLUT_INCLUDE_DIR})
message("-- FreeGLUT library: " ${FREEGLUT_LIBRARY})

find_package(MinVR MODULE)
message("-- MinVR includes:   " ${MINVR_INCLUDE_DIR})
message("-- MinVR library:    " ${MINVR_LIBRARY})


if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  # Linux specific code -- Not sure why pthreads or dl is required.  This
  # is not necessary on all systems.
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ldl -fPIC -lpthread")
  
#  set(CMAKE_C_COMPILER gcc)
#  set(CMAKE_CXX_COMPILER g++)

endif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  # Apple-specific code
  set(CMAKE_CXX_FLAGS "-DOSX")
endif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set (CMAKE_INSTALL_PREFIX "${${PROJECT_NAME}_SOURCE_DIR}/build/install"
    CACHE PATH "default install path" FORCE )
endif()

set(img_src_dir ${CMAKE_CURRENT_SOURCE_DIR}/src)

# add_subdirectory(src)
add_subdirectory(examples)

