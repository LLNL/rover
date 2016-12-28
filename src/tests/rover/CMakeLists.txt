###############################################################################
#
#  ROVER HEADER
#
###############################################################################


####################################
# Unit tests
####################################
set(BASIC_TESTS t_rover_smoke)

message(STATUS "Adding rover unit tests")
foreach(TEST ${BASIC_TESTS})
  add_cpp_test(TEST ${BASIC_TESTS} DEPENDS_ON rover)
endforeach()

