//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
// Copyright (c) 2018, Lawrence Livermore National Security, LLC.
// 
// Produced at the Lawrence Livermore National Laboratory
// 
// LLNL-CODE-749865
// 
// All rights reserved.
// 
// This file is part of Rover. 
// 
// Please also read rover/LICENSE
// 
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are met:
// 
// * Redistributions of source code must retain the above copyright notice, 
//   this list of conditions and the disclaimer below.
// 
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the disclaimer (as noted below) in the
//   documentation and/or other materials provided with the distribution.
// 
// * Neither the name of the LLNS/LLNL nor the names of its contributors may
//   be used to endorse or promote products derived from this software without
//   specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL LAWRENCE LIVERMORE NATIONAL SECURITY,
// LLC, THE U.S. DEPARTMENT OF ENERGY OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
// DAMAGES  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
// OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
// IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
// POSSIBILITY OF SUCH DAMAGE.
// 
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
#include <volume_engine.hpp>
#include <rover_exceptions.hpp>
#include <utils/rover_logging.hpp>
namespace rover {

VolumeEngine::VolumeEngine()
{
  m_tracer = NULL;
}

VolumeEngine::~VolumeEngine()
{
  if(m_tracer) delete m_tracer;
}

void
VolumeEngine::set_data_set(vtkm::cont::DataSet &dataset)
{
  if(m_tracer) delete m_tracer;
  m_tracer = new vtkm::rendering::ConnectivityProxy(dataset);
}


void 
VolumeEngine::set_primary_field(const std::string &primary_field)
{
  m_primary_field = primary_field;
  m_tracer->SetScalarField(this->m_primary_field);
}

void 
VolumeEngine::init_rays(Ray32 &rays)
{
  vtkm::cont::ArrayHandle<vtkm::Float32> signature;
  signature.Allocate(4);
  signature.GetPortalControl().Set(0,1.f);
  signature.GetPortalControl().Set(1,1.f);
  signature.GetPortalControl().Set(2,1.f);
  signature.GetPortalControl().Set(3,0.f);
  rays.Buffers.at(0).InitChannels(signature);
}

void 
VolumeEngine::init_rays(Ray64 &rays)
{
  vtkm::cont::ArrayHandle<vtkm::Float64> signature;
  signature.Allocate(4);
  signature.GetPortalControl().Set(0,1.);
  signature.GetPortalControl().Set(1,1.);
  signature.GetPortalControl().Set(2,1.);
  signature.GetPortalControl().Set(3,0.);
  rays.Buffers.at(0).InitChannels(signature);
}

void 
VolumeEngine::trace(Ray32 &rays)
{
  if(m_tracer == NULL)
  {
    std::cout<<"Volume Engine Error: must set the data set before tracing\n";
  }
  
  if(this->m_primary_field == "")
  {
    throw RoverException("Primary field is not set. Unable to render\n");
  }
  else
  {
    //m_tracer->SetScalarField(this->m_primary_field);
  }
  ROVER_INFO("tracing  rays");
  rays.Buffers.at(0).InitConst(0.);
  m_tracer->SetColorMap(m_color_map);
  m_tracer->Trace(rays);
}

void 
VolumeEngine::trace(Ray64 &rays)
{
  if(m_tracer == NULL)
  {
    std::cout<<"Volume Engine Error: must set the data set before tracing\n";
  }

  if(this->m_primary_field == "")
  {
    throw RoverException("Primary field is not set. Unable to render\n");
  }
  else
  {
    m_tracer->SetScalarField(this->m_primary_field);
  }

  ROVER_INFO("tracing  rays");
  rays.Buffers.at(0).InitConst(0.);
  m_tracer->SetColorMap(m_color_map);
  m_tracer->Trace(rays);
}

vtkmRange
VolumeEngine::get_primary_range()
{
  return m_tracer->GetScalarFieldRange();
}

void 
VolumeEngine::set_composite_background(bool on)
{
  m_tracer->SetCompositeBackground(on);
};

void
VolumeEngine::set_primary_range(const vtkmRange &range)
{
  return m_tracer->SetScalarRange(range);
}

void
VolumeEngine::set_samples(const vtkm::Bounds &global_bounds, const int &samples)
{
  const vtkm::Float32 num_samples = static_cast<float>(samples);
  vtkm::Vec<vtkm::Float32,3> totalExtent;
  totalExtent[0] = vtkm::Float32(global_bounds.X.Max - global_bounds.X.Min);
  totalExtent[1] = vtkm::Float32(global_bounds.Y.Max - global_bounds.Y.Min);
  totalExtent[2] = vtkm::Float32(global_bounds.Z.Max - global_bounds.Z.Min);
  vtkm::Float32 sample_distance = vtkm::Magnitude(totalExtent) / num_samples;
  m_tracer->SetSampleDistance(sample_distance);
}
  
}; //namespace rover
