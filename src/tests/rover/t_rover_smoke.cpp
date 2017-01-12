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
  Rover32 driver32;
  driver32.about();  

  Rover64 driver64;
  driver64.about();
}

