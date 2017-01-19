###############################################################################
#
#  ROVER HEADER
#
###############################################################################

set(DATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/data/")
configure_file(test_config.h.in "${CMAKE_CURRENT_SOURCE_DIR}/test_config.h" @ONLY)
####################################
# Unit tests
####################################
set(BASIC_TESTS t_rover_smoke
                t_rover_hex)

message(STATUS "Adding rover unit tests")
foreach(TEST ${BASIC_TESTS})
  message(STATUS "Adding rover unit test: ${TEST}")
  add_cpp_test(TEST ${TEST} DEPENDS_ON rover)
endforeach()

