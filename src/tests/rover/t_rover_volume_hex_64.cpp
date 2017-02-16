/*******************************************************************************
*
*  ROVER HEADER
*
*******************************************************************************/

#include <gtest/gtest.h>
#include <iostream>
#include "test_utils.hpp"
#include <rover.hpp>
#include <rover_exceptions.hpp>
#include <ray_generators/camera_generator.hpp>

using namespace rover;


TEST(rover_hex, test_call)
{

  try {
  vtkmCamera camera;
  vtkmDataSet dataset;
  set_up_lulesh(dataset, camera);

  CameraGenerator64 generator(camera);
  Rover64 driver64;
  //
  // Create some basic setting and color table
  //
  RenderSettings settings;
  settings.m_primary_field = "speed";
  vtkmColorTable color_table("cool2warm");
  color_table.AddAlphaControlPoint(0.0, .01);
  color_table.AddAlphaControlPoint(0.5, .02);
  color_table.AddAlphaControlPoint(1.0, .01);
  settings.m_color_table = color_table;
  
  driver64.set_render_settings(settings);
  driver64.add_data_set(dataset);
  driver64.set_ray_generator(&generator);
  driver64.execute();
  driver64.save_png("volume_hex_64");

  }
  catch ( const RoverException &e )
  {
    std::cout<<e.what();
    ASSERT_EQ("rover_exception", "it_happened");
  }
  catch (vtkm::cont::Error error)
  {
    std::cout<<"VTKM exception "<<error.GetMessage()<<"\n";
    ASSERT_EQ("vtkm_exception", "it_happened");
  }
  
  //Rover64 driver64;
  //driver64.about();
}

