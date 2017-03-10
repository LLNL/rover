###############################################################################
#
#
#
###############################################################################

add_subdirectory(../thirdparty_builtin/lodepng/ ${CMAKE_CURRENT_SOURCE_DIR}/utils/lodepng)

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
    compositing/volume_block.hpp
    compositing/absorption_block.hpp
    # engines
    engine.hpp
    energy_engine.hpp
    volume_engine.hpp
    # ray generators headers
    ray_generators/ray_generator.hpp
    ray_generators/camera_generator.hpp
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

###################################################
#   Build parallel version
##################################################

if(MPI_FOUND)
  message(STATUS "Building parallel rover")

  set(compositing_headers compositing/volume_redistribute.hpp
                          compositing/absorption_redistribute.hpp)
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

endif()
