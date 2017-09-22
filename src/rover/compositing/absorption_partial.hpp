#ifndef rover_absorption_partial_h
#define rover_absorption_partial_h

#include <assert.h>
#include <rover_types.hpp>

namespace rover {

template<typename FloatType>
struct AbsorptionPartial
{
  typedef FloatType ValueType;
  int                    m_pixel_id;
  double                 m_depth; 
  std::vector<FloatType> m_bins;
  FloatType              m_path_length;

  AbsorptionPartial()
    : m_pixel_id(0),
      m_depth(0.f),
      m_path_length(-1.f)
  {}

  void print()
  {

  }
  
  bool operator < (const AbsorptionPartial<FloatType> &other) const
  {
    //
    // In absorption only we can blend the same
    // pixel ids in any order
    //
    return m_pixel_id < other.m_pixel_id;
  }

  inline void blend(const AbsorptionPartial<FloatType> &other)
  {
    const int num_bins = m_bins.size();
    assert(num_bins == other.m_bins.size());
    m_path_length += other.m_path_length;
    for(int i = 0; i < num_bins; ++i)
    {
      m_bins[i] *= other.m_bins[i];
    }
  }

  inline void load_from_partial(const PartialImage<FloatType> &partial_image, const int &index)
  {
    const int num_bins= partial_image.m_buffer.GetNumChannels(); 
    m_pixel_id = static_cast<int>(partial_image.m_pixel_ids.GetPortalConstControl().Get(index));
    m_depth = partial_image.m_distances.GetPortalConstControl().Get(index);
    m_bins.resize(num_bins);

    bool has_path_length = partial_image.m_path_lengths.GetNumberOfValues() != 0;
    if(has_path_length)
    {
      m_path_length = partial_image.m_path_lengths.GetPortalConstControl().Get(index);
    }

    const int starting_index = index * num_bins;
    for(int i = 0; i < num_bins; ++i)
    {
      m_bins[i] = partial_image.m_buffer.Buffer.GetPortalConstControl().Get(starting_index + i);
    }
  }
  
  inline void store_into_partial(PartialImage<FloatType> &output, 
                                 const int &index,
                                 const std::vector<FloatType> &background)
  {
    const int num_bins = m_bins.size();
    output.m_pixel_ids.GetPortalControl().Set(index, m_pixel_id ); 
    output.m_distances.GetPortalControl().Set(index, m_depth ); 
    const int starting_index = num_bins * index;
    for(int  i = 0; i < num_bins; ++i)
    {
      output.m_buffer.Buffer.GetPortalControl().Set(starting_index + i, m_bins[i]);
      output.m_intensities.Buffer.GetPortalControl().Set(starting_index + i, m_bins[i] * background[i]);
    } 

    if(m_path_length >= 0.f)
    {
      output.m_path_lengths.GetPortalControl().Set(index, m_path_length); 
    }
  }

  static void composite_background(std::vector<AbsorptionPartial> &partials, 
                                   const std::vector<FloatType> &background)
  {
    const int size = static_cast<int>(partials.size());
    AbsorptionPartial<FloatType> bg;
    bg.m_path_length = 0.f;
    bg.m_bins = background;
    #pragma omp parallel for
    for(int i = 0; i < size; ++i)
    {
      partials[i].blend(bg); 
    }
  }

};

} // namespace rover


#endif
