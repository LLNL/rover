###############################################################################
##  Copyright (c) 2018, Lawrence Livermore National Security, LLC.
##  
##  Produced at the Lawrence Livermore National Laboratory
##  
##  LLNL-CODE-749865
##  
##  All rights reserved.
##  
##  This file is part of Rover. 
##  
##  Please also read rover/LICENSE
##  
##  Redistribution and use in source and binary forms, with or without 
##  modification, are permitted provided that the following conditions are met:
##  
##  * Redistributions of source code must retain the above copyright notice, 
##    this list of conditions and the disclaimer below.
##  
##  * Redistributions in binary form must reproduce the above copyright notice,
##    this list of conditions and the disclaimer (as noted below) in the
##    documentation and/or other materials provided with the distribution.
##  
##  * Neither the name of the LLNS/LLNL nor the names of its contributors may
##    be used to endorse or promote products derived from this software without
##    specific prior written permission.
##  
##  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
##  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
##  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
##  ARE DISCLAIMED. IN NO EVENT SHALL LAWRENCE LIVERMORE NATIONAL SECURITY,
##  LLC, THE U.S. DEPARTMENT OF ENERGY OR CONTRIBUTORS BE LIABLE FOR ANY
##  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
##  DAMAGES  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
##  OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
##  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
##  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
##  IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
##  POSSIBILITY OF SUCH DAMAGE.
##  
###############################################################################

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






