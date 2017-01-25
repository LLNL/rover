/*******************************************************************************
*
*  ROVER HEADER
*
*******************************************************************************/

#include <gtest/gtest.h>
#include "test_config.h"
#include "vtkm_utils.hpp"
#include <iostream>
#include <rover.hpp>
#include <rover_exceptions.hpp>
#include <ray_generators/camera_generator.hpp>
#include <utils/vtk_dataset_reader.hpp>

using namespace rover;


TEST(rover_hex, test_call)
{

  try {
  VTKReader reader;
  std::string data_dir(DATA_DIR);
  std::string file("luleshSpeed.vtk");
  std::string file_name = data_dir + file;
  std::cout<<"Reading file "<<file_name<<"\n";
  reader.read_file(file_name);
  vtkmDataSet dataset = reader.get_data_set();
 
  dataset.PrintSummary(std::cout); 
  vtkmCamera camera;
  typedef vtkm::Vec<vtkm::Float32,3> Vec3f;

  Vec3f position(-2.5f, -2.5f, -2.5f); 
  Vec3f up(1.f, 0.f, 0.f); 
  Vec3f look_at(.5f, .5f, .5f);
  const int image_width = 500;
  const int image_height = 500;
    
  camera.SetHeight(image_height);
  camera.SetWidth(image_width);
  camera.SetLookAt(look_at);
  camera.SetPosition(position);
  camera.SetUp(up);
  CameraGenerator64 generator(camera,
                              dataset.GetCoordinateSystem() );
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
  driver64.set_data_set(dataset);
  driver64.set_ray_generator(&generator);
  driver64.execute();
  driver64.save_png("volume_hex_64.png");

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

