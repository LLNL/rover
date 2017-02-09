###############################################################################
#
# Rover Header
# 
###############################################################################


################################
#  Project Wide Includes
################################

# add lodepng include dir
include_directories(${PROJECT_SOURCE_DIR}/thirdparty_builtin/lodepng)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty_builtin/gtest-1.7.0/include)

# add include dirs so units tests have access to the headers across
# libs and in unit tests

include_directories(${PROJECT_SOURCE_DIR}/rover/)

include_directories(${VTKm_INCLUDE_DIRS})

if(TBB_DIR)
  include(CMake/SetupTBB.cmake)
  if(TBB_FOUND)
    include_directories(${TBB_INCLUDE_DIRS})
  endif()
endif()

if(MPI_FOUND)
    include_directories(${MPI_CXX_INCLUDE_PATH})
endif()





