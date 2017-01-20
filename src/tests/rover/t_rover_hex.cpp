/*******************************************************************************
*
*  ROVER HEADER
*
*******************************************************************************/

#include <gtest/gtest.h>
#include "test_config.h"
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
  
  vtkmCamera camera;
  typedef vtkm::Vec<vtkm::Float32,3> Vec3f;

  Vec3f position(-1.f, -1.f, -1.f); 
  Vec3f up(1.f, 0.f, 0.f); 
  Vec3f look_at(.5f, .5f, .5f);
  const int image_width = 500;
  const int image_height = 500;
  
  camera.SetHeight(image_height);
  camera.SetWidth(image_width);
  camera.SetLookAt(look_at);
  camera.SetPosition(position);
  camera.SetUp(up);
  CameraGenerator32 generator(camera,
                              dataset.GetCoordinateSystem() );
  Rover32 driver32;
  RenderSettings settings;
  settings.m_primary_field = "speed";
  driver32.set_render_settings(settings);
  driver32.set_data_set(dataset);
  driver32.set_ray_generator(&generator);
  driver32.execute();
  driver32.save_png("hex32.png");
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

