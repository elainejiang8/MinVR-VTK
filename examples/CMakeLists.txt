# Use this file for compiling the examples

include_directories(${CMAKE_SOURCE_DIR}/src
		    ${MINVR_INCLUDE_DIR} 
		    ${OPENGL_INCLUDE_DIR}
		    ${FREEGLUT_INCLUDE_DIR}
		    ${GLEW_LIBRARY}
	 	    ${GLM_INCLUDE_DIR}
)


add_executable(objimporter objimporter.cxx)

target_link_libraries(objimporter 
	${VTK_LIBRARIES}
)

add_executable(demo demo.cxx)

target_link_libraries(demo 
	${VTK_LIBRARIES}
)

add_executable(density density.cxx)

target_link_libraries(density 
	${VTK_LIBRARIES}
)


add_executable(supernova supernova.cxx)

target_link_libraries(supernova minvr-vtk
	${VTK_LIBRARIES}
)

add_executable(objimporter2 objimporter2.cxx)

target_link_libraries(objimporter2
	${VTK_LIBRARIES} 
	${MINVR_LIBRARY}
	${OPENGL_LIBRARY}
	${FREEGLUT_LIBRARY}
	${GLEW_LIBRARY}
)

#add_executable(demo3 demo3.cxx)

#target_link_libraries(demo3
#	${VTK_LIBRARIES} 
#	${MINVR_LIBRARY}
#	${OPENGL_LIBRARY}
#	${FREEGLUT_LIBRARY}
#	${GLEW_LIBRARY}
#)


add_executable(supernova3 supernova3.cxx)

target_link_libraries(supernova3 minvr-vtk
	${VTK_LIBRARIES} 
	${MINVR_LIBRARY}
	${OPENGL_LIBRARY}
	${FREEGLUT_LIBRARY}
	${GLEW_LIBRARY}
)




  
