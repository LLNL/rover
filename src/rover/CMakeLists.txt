###############################################################################
#
#
#
###############################################################################

add_subdirectory(../thirdparty_builtin/lodepng/ ${CMAKE_CURRENT_SOURCE_DIR}/utils)

set(rover_headers
    compositor.hpp
    engine.hpp
    rover_exports.h
    rover_exceptions.hpp
    rover_types.hpp
    rover.hpp
    schedular.hpp
    static_schedular.hpp
    volume_engine.hpp
    # ray generators headers
    ray_generators/ray_generator.hpp
    ray_generators/camera_generator.hpp
    utils/png_encoder.hpp
   )

set(rover_sources
    rover.cpp
    schedular.cpp
    volume_engine.cpp
    # ray generators
    ray_generators/ray_generator.cpp
    ray_generators/camera_generator.cpp
    utils/png_encoder.cpp
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
              utils/<TARGET_OBJECTS:lodepng>
              )
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(rover PRIVATE ${VTKm_INCLUDE_DIRS})
target_link_libraries(rover ${VTKm_LIBRARIES})
