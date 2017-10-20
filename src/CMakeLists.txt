project(minvr-vtk)

include_directories(
  ${OPENGL_INCLUDE_DIR}
  ${FREEGLUT_INCLUDE_DIR}
  ${GLM_INCLUDE_DIR}
  ${GLEW_INCLUDE_DIRS}
  )

set(headers vtkCustomExternalOpenGLCamera.h vtkCustomPropPicker.h)
set(sources vtkCustomExternalOpenGLCamera.cxx vtkCustomPropPicker.cxx)
set(files ${headers} ${sources})

add_library(minvr-vtk ${files})


install(TARGETS minvr-vtk
  LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
	ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

install(FILES ${headers}
  DESTINATION ${CMAKE_INSTALL_PREFIX}/include)

