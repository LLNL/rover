cmake_minimum_required(VERSION 3.0)

project(using_with_cmake)

include ("FindRover.cmake")

include_directories(${ROVER_INCLUDE_DIRS})
message(STATUS "VTKm dirs ${VTKm_INCLUDE_DIRS}")
add_executable(example example.cpp)

target_link_libraries(example rover)
