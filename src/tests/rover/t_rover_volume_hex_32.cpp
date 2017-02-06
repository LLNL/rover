/*******************************************************************************
*
*  ROVER HEADER
*
*******************************************************************************/

#include <gtest/gtest.h>
#include "test_utils.hpp"
#include <iostream>
#include <rover.hpp>
#include <rover_exceptions.hpp>
#include <ray_generators/camera_generator.hpp>
#include <utils/vtk_dataset_reader.hpp>

using namespace rover;


TEST(rover_hex, test_call)
{

  try {
  vtkmCamera camera;
  vtkmDataSet dataset;
  set_up_lulesh(dataset, camera);

  CameraGenerator32 generator(camera,
                              dataset.GetCoordinateSystem() );
  Rover32 driver32;
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
  
  driver32.set_render_settings(settings);
  driver32.add_data_set(dataset);
  driver32.set_ray_generator(&generator);
  driver32.execute();
  driver32.save_png("volume_hex_32");

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
  
}

