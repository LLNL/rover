###############################################################################
#
#
#
###############################################################################

add_subdirectory(../thirdparty_builtin/lodepng/ ${CMAKE_CURRENT_SOURCE_DIR}/utils/lodepng)

set(targets_export_name "RoverTargets")

set(rover_headers
    domain.hpp
    image.hpp
    rover_exports.h
    rover_exceptions.hpp
    rover_types.hpp
    rover.hpp
    scheduler.hpp
    scheduler_base.hpp
    static_scheduler.hpp
    # compositing
    compositing/compositor.hpp
    compositing/volume_partial.hpp
    compositing/absorption_partial.hpp
    # engines
    engine.hpp
    energy_engine.hpp
    volume_engine.hpp
    # ray generators headers
    ray_generators/ray_generator.hpp
    ray_generators/camera_generator.hpp
    ray_generators/visit_generator.hpp
    vtkm_typedefs.hpp
    # utils headers
    utils/png_encoder.hpp
    utils/rover_logging.hpp
    utils/vtk_dataset_reader.hpp
   )

set(rover_sources
    domain.cpp
    image.cpp
    rover.cpp
    scheduler.cpp
    scheduler_base.cpp
    # compositing
    compositing/compositor.cpp
    # engines
    energy_engine.cpp
    volume_engine.cpp
    # ray generators
    ray_generators/ray_generator.cpp
    ray_generators/camera_generator.cpp
    ray_generators/visit_generator.cpp
    # utils sources
    utils/png_encoder.cpp
    utils/rover_logging.cpp
    utils/vtk_dataset_reader.cpp
   )

   
if(BUILD_SHARED_LIBS)
  message(STATUS "Building rover as a shared library")

  add_library(rover SHARED
              ${rover_sources}
              ${rover_headers}
              $<TARGET_OBJECTS:lodepng>
              )
else()
  message(STATUS "Building rover as a static library")

  add_library(rover STATIC
              ${rover_sources}
              ${rover_headers}
              $<TARGET_OBJECTS:lodepng>
              )
endif()

##########################################
# Ensure we use c++11
##########################################

set_target_properties(rover PROPERTIES
                      CXX_STANDARD 11
                      CXX_STANDARD_REQUIRED YES
                      CXX_EXTENTIONS NO)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty_builtin/diy2/include/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# PUBLIC keyword pushes these libs into RoverTargets.cmake
# and they are automatically pulled into downstream projects
target_link_libraries(rover PUBLIC ${VTKm_LIBRARIES})
target_include_directories(rover PUBLIC ${VTKm_INCLUDE_DIRS})

if(TBB_FOUND)
  target_link_libraries(rover PUBLIC ${TBB_LIBRARIES})
  target_include_directories(rover PUBLIC ${TBB_INCLUDE_DIRS})
endif()

# install target for mpi ver
install(TARGETS rover
         #EXPORT rover 
        EXPORT ${targets_export_name} 
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION lib
        INCLUDES DESTINATION include/rover  # this exports the includes into 
                                            # the RoverTargets.cmake
)
###################################################
#   Build parallel version
##################################################

if(MPI_FOUND)
  message(STATUS "Building parallel rover")

  set(compositing_headers compositing/blocks.hpp
                          compositing/collect.hpp
                          compositing/redistribute.hpp)
  list(APPEND rover_headers ${compositing_headers})
  set(DIY_DIR "../thirdparty_builtin/diy2/include")


  if(BUILD_SHARED_LIBS)
    message(STATUS "Building rover_par as a shared library")

    add_library(rover_par SHARED
                ${rover_sources}
                ${rover_headers}
                $<TARGET_OBJECTS:lodepng>
                )
  else()
    message(STATUS "Building rover_par as a static library")

    add_library(rover_par STATIC
                ${rover_sources}
                ${rover_headers}
                $<TARGET_OBJECTS:lodepng>
                )
  endif()
  
  ##########################################
  # Ensure we use c++11
  ##########################################

  set_target_properties(rover_par PROPERTIES
                        CXX_STANDARD 11
                        CXX_STANDARD_REQUIRED YES
                        CXX_EXTENTIONS NO)

  add_target_compile_flags(TARGET rover_par
                           FLAGS "${MPI_CXX_COMPILE_FLAGS} -D PARALLEL")
  target_include_directories(rover_par PRIVATE ${DIY_DIR})
  target_include_directories(rover_par PRIVATE ${MPI_INCLUDE_PATH})
  target_include_directories(rover_par PUBLIC ${VTKm_INCLUDE_DIRS})
  target_link_libraries(rover_par PUBLIC ${VTKm_LIBRARIES} ${MPI_CXX_LIBRARIES})

  if(TBB_FOUND)
    target_link_libraries(rover_par PUBLIC ${TBB_LIBRARIES})
    target_include_directories(rover_par PUBLIC ${TBB_INCLUDE_DIRS})
  endif()
endif()

  # install target for mpi ver
  install(TARGETS rover_par
          #EXPORT rover 
          EXPORT ${targets_export_name}  
          LIBRARY DESTINATION lib
          ARCHIVE DESTINATION lib
          RUNTIME DESTINATION lib
          INCLUDES DESTINATION include/rover  # this exports the includes into 
                                              # the RoverTargets.cmake
  )

########################################
# Extra Install Targets for rover 
########################################
include(CMakePackageConfigHelpers)

configure_package_config_file(
  "../CMake/RoverConfig.cmake.in" ${CMAKE_CURRENT_BINARY_DIR}/RoverConfig.cmake
  INSTALL_DESTINATION "/lib" )


########################################
# Preserve install directory structure 
########################################
macro(install_headers_with_directory header_list)

  foreach(header ${${header_list}})
    string(REGEX MATCH "(.*)[/\\]" DIR ${header}) 
    install(FILES ${header} DESTINATION include/rover/${DIR})
  endforeach(header)

endmacro(install_headers_with_directory)

install(EXPORT "${targets_export_name}" DESTINATION "lib/")
install_headers_with_directory(rover_headers)
#install(FILES ${rover_headers} DESTINATION include/rover)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/RoverConfig.cmake  DESTINATION lib/)
