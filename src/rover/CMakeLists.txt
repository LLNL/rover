###############################################################################
#
#
#
###############################################################################


set(rover_headers
    rover_exports.h
    rover.hpp
   )

set(rover_sources
    rover.cpp
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
