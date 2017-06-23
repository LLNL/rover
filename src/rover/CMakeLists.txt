###############################################################################
#
#
#
###############################################################################

add_subdirectory(../thirdparty_builtin/lodepng/ ${CMAKE_CURRENT_SOURCE_DIR}/utils/lodepng)

set(targets_export_name "${PROJECT_NAME}Targets")

set(rover_headers
    domain.hpp
    rover_exports.h
    rover_exceptions.hpp
    rover_types.hpp
    rover.hpp
    scheduler.hpp
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
    vtkm_typedefs.hpp
    # utils headers
    utils/png_encoder.hpp
    utils/rover_logging.hpp
    utils/vtk_dataset_reader.hpp
   )

set(rover_sources
    domain.cpp
    rover.cpp
    scheduler.cpp
    # compositing
    compositing/compositor.cpp
    # engines
    energy_engine.cpp
    volume_engine.cpp
    # ray generators
    ray_generators/ray_generator.cpp
    ray_generators/camera_generator.cpp
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

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty_builtin/diy2/include/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(rover PRIVATE ${VTKm_INCLUDE_DIRS})
target_link_libraries(rover ${VTKm_LIBRARIES})

if(TBB_FOUND)
   target_link_libraries(rover ${TBB_LIBRARIES})
endif()

# install target for mpi ver
install(TARGETS rover
         #EXPORT rover 
        EXPORT ${targets_export_name} 
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION lib
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
  
  add_target_compile_flags(TARGET rover_par
                           FLAGS "${MPI_CXX_COMPILE_FLAGS} -D PARALLEL")
                         target_include_directories(rover_par PRIVATE ${VTKm_INCLUDE_DIRS} ${DIY_DIR})
  target_link_libraries(rover_par ${VTKm_LIBRARIES} ${MPI_CXX_LIBRARIES})

  if(TBB_FOUND)
     target_link_libraries(rover_par ${TBB_LIBRARIES})
  endif()
endif()

  # install target for mpi ver
  install(TARGETS rover_par
          #EXPORT rover 
          EXPORT ${targets_export_name}  
          LIBRARY DESTINATION lib
          ARCHIVE DESTINATION lib
          RUNTIME DESTINATION lib
  )

########################################
# Extra Install Targets for rover 
########################################
include(CMakePackageConfigHelpers)

configure_package_config_file(
  "../CMake/RoverConfig.cmake.in" ${CMAKE_CURRENT_BINARY_DIR}/RoverConfig.cmake
  INSTALL_DESTINATION "/lib" )


install(EXPORT "${targets_export_name}" DESTINATION "lib/")
install(FILES ${rover_headers} DESTINATION include/rover)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/RoverConfig.cmake  DESTINATION lib/)
