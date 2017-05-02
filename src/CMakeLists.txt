###############################################################################
#
#  ROVER HEADER
#
###############################################################################

cmake_minimum_required(VERSION 3.3)

####################################
# Rover
####################################

project(rover)

####################################
# Build Options 
####################################

option(BUILD_SHARED_LIBS "Build shared libraries"    ON)
option(ENABLE_TESTS      "Build tests"               ON)
option(ENABLE_CUDA       "Build CUDA support"        OFF)
option(ENABLE_MPI        "Build MPI Support"         OFF)
option(ENABLE_TBB        "Build TBB support"         OFF)


####################################
# Find VTK-m 
####################################
if(NOT VTKM_DIR)
  MESSAGE(FATAL_ERROR "Rover needs explicit VTKM_DIR")
endif()

set(VTKm_DIR ${VTKM_DIR}/lib)

if(TBB_DIR)
  set(ENV{TBB_ROOT} ${TBB_DIR})
endif()

find_package(VTKm REQUIRED OPTIONAL_COMPONENTS Rendering)
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
# set the cxx flags 
####################################
set(CMAKE_CXX_FLAGS "${CMAKE_CXXFLAGS} -std=c++11")

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
