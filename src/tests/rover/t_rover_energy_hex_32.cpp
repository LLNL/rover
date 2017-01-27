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
  reader.read_file(file_name) ;
  vtkmDataSet dataset = reader.get_data_set();
  dataset.PrintSummary(std::cout); 
  const int num_bins = 100; 
  add_absorption_field(dataset, "speed", num_bins, vtkm::Float32());
  vtkmCamera camera;
  typedef vtkm::Vec<vtkm::Float32,3> Vec3f;

  Vec3f position(-2.5f, -2.5f, -2.5f); 
  Vec3f up(1.f, 0.f, 0.f); 
  Vec3f look_at(.5f, .5f, .5f);
  const int image_width = 500;
  const int image_height = 500;
  //
  // Create a camera
  //
  camera.SetHeight(image_height);
  camera.SetWidth(image_width);
  camera.SetLookAt(look_at);
  camera.SetPosition(position);
  camera.SetUp(up);
  CameraGenerator32 generator(camera,
                              dataset.GetCoordinateSystem() );
  Rover32 driver32;
  //
  // Create some basic setting and color table
  //
  RenderSettings settings;
  settings.m_primary_field = "absorption";
  settings.m_render_mode = rover::energy;

  driver32.set_render_settings(settings);
  driver32.set_data_set(dataset);
  driver32.set_ray_generator(&generator);
  driver32.execute();
  driver32.save_png("hex32");

  }
  catch ( const RoverException &e )
  {
    std::cout<<e.what();
  }
  catch (vtkm::cont::Error error)
  {
    std::cout<<"VTKM exception "<<error.GetMessage()<<"\n";
  }
  
  //Rover64 driver64;
  //driver64.about();
}

