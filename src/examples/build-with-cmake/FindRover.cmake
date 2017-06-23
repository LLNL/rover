if(NOT ROVER_DIR)
  message(FATAL_ERROR "Could not find Rover. Rover needs explicit ROVER_DIR to be set")
endif()

if(NOT EXISTS ${ROVER_DIR}/lib/RoverConfig.cmake)
  message(FATAL_ERROR "Could not find Rover CMake file: ${ROVER_DIR}/lib/RoverConifg.cmake")
endif()

if(NOT EXISTS ${ROVER_DIR}/lib/roverTargets.cmake)
  message(FATAL_ERROR "Could not find Rover CMake file: ${ROVER_DIR}/lib/roverTargets.cmake")
endif()

include(${ROVER_DIR}/lib/roverTargets.cmake)

include(${ROVER_DIR}/lib/RoverConfig.cmake)

set(ROVER_FOUND TRUE)
message(STATUS "Found Rover. Include dirs ${ROVER_INCLUDE_DIRS}")
