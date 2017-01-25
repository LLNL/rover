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
                t_rover_volume_hex_32
                t_rover_volume_hex_64
                t_rover_energy_hex_32)

message(STATUS "Adding rover unit tests")
foreach(TEST ${BASIC_TESTS})
  message(STATUS "Adding rover unit test: ${TEST}")
  add_cpp_test(TEST ${TEST} DEPENDS_ON rover)
endforeach()

