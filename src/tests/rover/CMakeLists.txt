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
                t_rover_multi_volume_hex_32
                t_rover_multi_empty
                t_rover_volume_uni_32
                t_rover_volume_hex_32
                t_rover_volume_hex_64
                t_rover_energy_hex_32
                t_rover_energy_clock
                t_rover_energy_hardy
                t_rover_energy_emission_hex_32
                t_rover_energy_hex_64
                t_rover_energy_zoo_32
                t_rover_volume_zoo_32)

set(MPI_TESTS t_rover_multi_volume_hex_32_par
              t_rover_multi_energy_hex_32_par
              t_rover_multi_energy_empty_domain_par
              t_rover_multi_nyx_par
              t_rover_multi_energy_emission_hex_32_par)

message(STATUS "Adding rover unit tests")

#####################################
# Adding basic tests
#####################################
foreach(TEST ${BASIC_TESTS})
  message(STATUS "Adding rover unit test: ${TEST}")
  add_cpp_test(TEST ${TEST} DEPENDS_ON rover)
endforeach()

#####################################
# Adding mpi tests
#####################################
if(MPI_FOUND)
  foreach(TEST ${MPI_TESTS})
    message(STATUS "Adding rover parallel unit test: ${TEST}")
    add_cpp_mpi_test(TEST ${TEST} NUM_PROCS 2 DEPENDS_ON rover_par)
    set_target_properties( ${TEST} PROPERTIES COMPILE_FLAGS "-D PARALLEL")
  endforeach()
endif()
