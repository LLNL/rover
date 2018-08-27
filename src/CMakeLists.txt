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

cmake_minimum_required(VERSION 3.3)

####################################
# Rover
####################################

project(rover)
set(ROVER_MAJOR_VERSION 0)
set(ROVER_MINOR_VERSION 1)
set(ROVER_PATCH_VERSION 0)
set(ROVER_VERSION
    ${ROVER_MAJOR_VERSION}.${ROVER_MINOR_VERSION}.${ROVER_PATCH_VERSION}
    )
####################################
# Build Options 
####################################

option(BUILD_SHARED_LIBS "Build shared libraries"    ON)
option(ENABLE_TESTS      "Build tests"               ON)
option(ENABLE_CUDA       "Build CUDA support"        OFF)
option(ENABLE_MPI        "Build MPI Support"         OFF)
option(ENABLE_LOGGING    "Rover logging"             OFF)

####################################
# Find VTK-m 
####################################
if(NOT VTKM_DIR)
    MESSAGE(FATAL_ERROR "VTKm support needs explicit VTKM_DIR")
endif()

MESSAGE(STATUS "Looking for VTKm using VTKM_DIR = ${VTKM_DIR}")

# use VTKM_DIR to setup the options that cmake's find VTKm needs
set(VTKm_DIR ${VTKM_DIR}/lib/cmake/vtkm-1.2)

set(VTKM_FOUND TRUE)
message(STATUS "Found VTKm Include Dirs: ${VTKm_INCLUDE_DIRS}")

set(VTKm_LIBRARIES vtkm_cont vtkm_rendering)

find_package(VTKm CONFIG REQUIRED OPTIONAL_COMPONENTS Rendering)
message(STATUS "Found VTKm Include Dirs: ${VTKm_INCLUDE_DIRS}")
set(VTKM_FOUND TRUE)

####################################
# Setup the basic cmake options 
####################################
include(CMake/CMakeBasics.cmake)

####################################
# Setup the tests 
####################################
include(CMake/SetupTests.cmake)

####################################
# Find CUDA 
####################################
if(ENABLE_CUDA)

  if(CUDA_BIN_DIR)
    set(ENV{CUDA_BIN_PATH} ${CUDA_BIN_DIR})
  endif()

  find_package(CUDA)
  if(NOT CUDA_FOUND)
    MESSAGE(FATAL_ERROR "ENABLE_CUDA is true, but a CUDA install was not found")
  endif()
endif()


####################################
# Find MPI 
####################################

if(ENABLE_MPI)
  include(FindMPI)
  if(NOT MPI_FOUND)
    message(FATAL_ERROR "Enable MPI is true, but MPI was not found")
  endif()
endif()

####################################
# Setup the project wide includes 
####################################
include(CMake/SetupIncludes.cmake)

####################################
# Add the thirdparty 
####################################
add_subdirectory(thirdparty_builtin)

####################################
# Add the rover dir 
####################################
add_subdirectory(rover)

####################################
# Add the examples dir 
####################################
add_subdirectory(examples)

####################################
# Add the test 
####################################
if(ENABLE_TESTS)
  add_subdirectory(tests)
endif()

