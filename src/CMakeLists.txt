set (CMAKE_CXX_STANDARD 11)

project(minvr-vtk)

include_directories(
  ${OPENGL_INCLUDE_DIR}
  ${FREEGLUT_INCLUDE_DIR}
  ${GLM_INCLUDE_DIR}
  ${GLEW_INCLUDE_DIRS}
  ${VTK_INCLUDE_DIRS}
  )

set(headers ActorWrapper.h MVRDemo.h)
set(sources ActorWrapper.cxx)
set(files ${sources})

add_library(minvr-vtk ${files})


install(TARGETS minvr-vtk
  LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
	ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

install(FILES ${headers}
  DESTINATION ${CMAKE_INSTALL_PREFIX}/include)

