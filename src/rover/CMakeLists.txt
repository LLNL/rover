###############################################################################
#
#
#
###############################################################################


set(rover_headers
    compositor.hpp
    engine.hpp
    rover_exports.h
    rover_types.hpp
    rover.hpp
    schedular.hpp
    static_schedular.hpp
    volume_engine.hpp
    # ray container headers
    ray_containers/ray_container.hpp
   )

set(rover_sources
    rover.cpp
    schedular.cpp
    volume_engine.cpp
   )

if(BUILD_SHARED_LIBS)
  message(STATUS "Building rover as a shared library")

  add_library(rover SHARED
              ${rover_sources}
              ${rover_headers}
              )
else()
  message(STATUS "Building rover as a static library")

  add_library(rover STATIC
              ${rover_sources}
              ${rover_headers}
              )
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(rover PRIVATE ${VTKm_INCLUDE_DIRS})
target_link_libraries(rover ${VTKm_LIBRARIES})
