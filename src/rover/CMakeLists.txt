###############################################################################
##  Copyright (c) 2018, Lawrence Livermore National Security, LLC.
##  
##  Produced at the Lawrence Livermore National Laboratory
##  
##  LLNL-CODE-749865
##  
##  All rights reserved.
##  
##  This file is part of Rover. 
##  
##  Please also read rover/LICENSE
##  
##  Redistribution and use in source and binary forms, with or without 
##  modification, are permitted provided that the following conditions are met:
##  
##  * Redistributions of source code must retain the above copyright notice, 
##    this list of conditions and the disclaimer below.
##  
##  * Redistributions in binary form must reproduce the above copyright notice,
##    this list of conditions and the disclaimer (as noted below) in the
##    documentation and/or other materials provided with the distribution.
##  
##  * Neither the name of the LLNS/LLNL nor the names of its contributors may
##    be used to endorse or promote products derived from this software without
##    specific prior written permission.
##  
##  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
##  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
##  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
##  ARE DISCLAIMED. IN NO EVENT SHALL LAWRENCE LIVERMORE NATIONAL SECURITY,
##  LLC, THE U.S. DEPARTMENT OF ENERGY OR CONTRIBUTORS BE LIABLE FOR ANY
##  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
##  DAMAGES  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
##  OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
##  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
##  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
##  IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
##  POSSIBILITY OF SUCH DAMAGE.
##  
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

if(ENABLE_LOGGING)
  target_compile_definitions(rover PRIVATE ROVER_ENABLE_LOGGING=1) 
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

  install(TARGETS rover_par
          #EXPORT rover 
          EXPORT ${targets_export_name}  
          LIBRARY DESTINATION lib
          ARCHIVE DESTINATION lib
          RUNTIME DESTINATION lib
          INCLUDES DESTINATION include/rover  # this exports the includes into 
                                              # the RoverTargets.cmake
  )
endif()

if(ENABLE_LOGGING)
  target_compile_definitions(rover_par PRIVATE ROVER_ENABLE_LOGGING=1) 
endif()

  # install target for mpi ver

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
