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
# Find VTK-m 
####################################
if(NOT VTKM_DIR)
  MESSAGE(FATAL_ERROR "Rover needs explicit VTKM_DIR")
endif()

set(VTKm_DIR ${VTKM_DIR}/lib)

if(TBB_DIR)
  set(ENV{TBB_ROOT} ${TBB_DIR})
endif()

find_package(VTKm REQUIRED)
message(STATUS "Found VTKm Include Dirs: ${VTKm_INCLUDE_DIRS}")
set(VTKM_FOUND TRUE)

####################################
# set the cxx flags 
####################################
set(CMAKE_CXX_FLAGS "${CMAKE_CXXFLAGS} -std=c++11")

####################################
# Add the test dir 
####################################
add_subdirectory(tests)

