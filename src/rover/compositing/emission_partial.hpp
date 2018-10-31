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
#ifndef rover_emission_partial_h
#define rover_emission_partial_h

#include <assert.h>
#include <rover_types.hpp>

namespace rover {

template<typename FloatType>
struct EmissionPartial
{
  typedef FloatType ValueType;

  int                    m_pixel_id;
  double                 m_depth; 
  std::vector<FloatType> m_bins;
  std::vector<FloatType> m_emission_bins;
  FloatType              m_path_length;

  EmissionPartial()
    : m_pixel_id(0),
      m_depth(0.f),
      m_path_length(-1.f)
  {

  }

  void alter_bin(int bin, FloatType value)
  {
    m_bins[bin] = value; 
    m_emission_bins[bin] = value; 
  }

  void print()
  {
    std::cout<<"Partial id "<<m_pixel_id<<"\n";
    std::cout<<"Absorption : ";
    for(int i = 0; i < m_bins.size(); ++i)
    {
      std::cout<<m_bins[i]<<" ";
    }
    std::cout<<"\n";
    std::cout<<"Emission: ";
    for(int i = 0; i < m_bins.size(); ++i)
    {
      std::cout<<m_emission_bins[i]<<" ";
    }
    std::cout<<"\n";
  }
  
  bool operator < (const EmissionPartial<FloatType> &other) const
  {
    if(m_pixel_id != other.m_pixel_id) 
    {
      return m_pixel_id < other.m_pixel_id;
    }
    else
    {
      return m_depth < other.m_depth;
    }
  }
  
  inline void blend_absorption(const EmissionPartial<FloatType> &other)
  {
    const int num_bins = static_cast<int>(m_bins.size());
    assert(num_bins == (int)other.m_bins.size());
    m_path_length += other.m_path_length;
    for(int i = 0; i < num_bins; ++i)
    {
      m_bins[i] *= other.m_bins[i];
    }
  }

  inline void blend_emission(EmissionPartial<FloatType> &other)
  {
    const int num_bins = static_cast<int>(m_bins.size());
    assert(num_bins == (int)other.m_bins.size());
    for(int i = 0; i < num_bins; ++i)
    {
      m_emission_bins[i] *= other.m_bins[i];
    }
  }

  inline void add_emission(EmissionPartial<FloatType> &other)
  {
    const int num_bins = static_cast<int>(m_bins.size());
    assert(num_bins == (int)other.m_bins.size());
    for(int i = 0; i < num_bins; ++i)
    {
      m_emission_bins[i] += other.m_emission_bins[i];
    }
  }

  inline void load_from_partial(const PartialImage<FloatType> &partial_image, const int &index)
  {
    const int num_bins= partial_image.m_buffer.GetNumChannels(); 
    m_pixel_id = static_cast<int>(partial_image.m_pixel_ids.GetPortalConstControl().Get(index));
    m_depth = partial_image.m_distances.GetPortalConstControl().Get(index);
    m_bins.resize(num_bins);
    m_emission_bins.resize(num_bins);

    bool has_path_length = partial_image.m_path_lengths.GetNumberOfValues() != 0;
    if(has_path_length)
    {
      m_path_length = partial_image.m_path_lengths.GetPortalConstControl().Get(index);
    }

    const int starting_index = index * num_bins;
    for(int i = 0; i < num_bins; ++i)
    {
      m_bins[i] = partial_image.m_buffer.Buffer.GetPortalConstControl().Get(starting_index + i);
      m_emission_bins[i] = partial_image.m_intensities.Buffer.GetPortalConstControl().Get(starting_index + i);
    }
  }
  
  inline void store_into_partial(PartialImage<FloatType> &output, 
                                 const int &index,
                                 const std::vector<FloatType> &background)
  {
    const int num_bins = static_cast<int>(m_bins.size());
    output.m_pixel_ids.GetPortalControl().Set(index, m_pixel_id ); 
    output.m_distances.GetPortalControl().Set(index, m_depth ); 
    const int starting_index = num_bins * index;
    for(int  i = 0; i < num_bins; ++i)
    {
      output.m_buffer.Buffer.GetPortalControl().Set(starting_index + i, m_bins[i]);
      FloatType out_intensity = m_emission_bins[i] +  m_bins[i] * background[i];
      output.m_intensities.Buffer.GetPortalControl().Set(starting_index + i, out_intensity);
    } 

    if(m_path_length >= 0.f)
    {
      output.m_path_lengths.GetPortalControl().Set(index, m_path_length); 
    }
  }

  static void composite_background(std::vector<EmissionPartial> &partials, 
                                   const std::vector<FloatType> &background)
  {
    //for(
  }

};

} // namespace rover


#endif
