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

if(MPI_FOUND)
    include_directories(${MPI_CXX_INCLUDE_PATH})
endif()





