###############################################################################
#
# Noise CMake Build for Rover
#
###############################################################################

###############################
# get access to the test utils
###############################
include_directories(${PROJECT_SOURCE_DIR}/tests/rover/)

set(noise_sources
    noise.cpp
    open_simplex_noise.c)


add_executable(noise_ser ${noise_sources} )

target_link_libraries(noise_ser rover ${VTKm_LIBRARIES})

if(OPENMP_FOUND)
  set(noise_openmp_flags "-DNOISE_USE_OPENMP")
else()
   set(noise_openmp_flags "")
endif()

add_target_compile_flags(TARGET noise_ser
                         FLAGS "${noise_openmp_flags}")

if(MPI_FOUND)
    add_executable(noise_par ${noise_sources})
    target_include_directories(noise_par PRIVATE ${MPI_INCLUDE_PATH})
    add_target_compile_flags(TARGET noise_par 
                             FLAGS "${MPI_CXX_COMPILE_FLAGS} -DPARALLEL  ${noise_openmp_flags}")

    add_target_link_flags(TARGET noise_par  
                          FLAGS "${MPI_CXX_LINK_FLAGS}")

    target_link_libraries(noise_par
                          ${VTKm_LIBRARIES}
                          rover_par
                          ${MPI_CXX_LIBRARIES})
endif()






