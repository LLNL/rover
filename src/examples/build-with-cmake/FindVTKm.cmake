###############################################################################
#
# Setup VTKm 
#
###############################################################################
#
#  Expects VTKM_DIR to point to a VTKm installation.
#
# This file defines the following CMake variables:
#  VTKM_FOUND - If VTKm was found
#  VTKM_INCLUDE_DIRS - The VTKm include directories
#
#  If found, the vtkm CMake targets will also be imported.
#  The main vtkm library targets are:
#   vtkm 
#   vtkm_cont
#   vtkm_rendering
#
###############################################################################

###############################################################################
# Check for VTKM_DIR
###############################################################################
if(NOT VTKM_DIR)
  MESSAGE(FATAL_ERROR "Could not find VTKM_DIR. Conduit requires explicit VTKM_DIR.")
endif()

if(NOT EXISTS ${VTKM_DIR}/lib/VTKmConfig.cmake)
  MESSAGE(FATAL_ERROR "Could not find VTKm CMake include file (${VTKM_DIR}/lib/VTKmConfig.cmake)")
endif()

###############################################################################
# Import VTKm CMake targets
###############################################################################
include(${VTKM_DIR}/lib/VTKmConfig.cmake)

###############################################################################
# Set remaning CMake variables 
###############################################################################
# we found VTKm
set(VTKM_FOUND TRUE)
# provide location of the headers in VTKM_INCLUDE_DIRS
set(VTKM_INCLUDE_DIRS ${VTKM_DIR}/include/vtkm)




