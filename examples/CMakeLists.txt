# Use this file for compiling the examples

include_directories(${MINVR_INCLUDE_DIR})


add_executable(MinIntensityRendering MinIntensityRendering.cxx)

target_link_libraries(MinIntensityRendering ${VTK_LIBRARIES})


add_executable(SmartVolumeMapper SmartVolumeMapper.cxx)

target_link_libraries(SmartVolumeMapper ${VTK_LIBRARIES})


add_executable(MinVRIntensityRendering MinVRIntensityRendering.cxx)

target_link_libraries(MinVRIntensityRendering ${VTK_LIBRARIES} ${MINVR_LIBRARY})


add_executable(demo demo.cxx)

target_link_libraries(demo ${VTK_LIBRARIES})


# add_executable(RenderInExistingContext RenderInExistingContext.cxx)

# target_link_libraries(RenderInExistingContext ${VTK_LIBRARIES})


  