/*******************************************************************************
*
*  ROVER HEADER
*
*******************************************************************************/

#include <gtest/gtest.h>
#include <iostream>
#include <rover.hpp>

using namespace rover;

TEST(rover_smoke, test_call)
{
  Rover driver32;
  driver32.about();  
}

