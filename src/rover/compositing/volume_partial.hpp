#ifndef rover_volume_block_h
#define rover_volume_block_h

#include <rover_types.hpp>

namespace rover {

struct VolumePartial
{
  int                    m_pixel_id;
  float                  m_depth; 
  unsigned char          m_pixel[3];
  float                  m_alpha;

  VolumePartial()
    : m_pixel_id(0),
      m_depth(0.f),
      m_alpha(0.f)
  {
    m_pixel[0] = 0;
    m_pixel[1] = 0;
    m_pixel[2] = 0;
  }

  bool operator < (const VolumePartial &other) const
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

  inline void blend(const VolumePartial &other)
  {
    if(m_alpha >= 1.f) return;
    const float opacity = (1.f - m_alpha) * other.m_alpha;
    m_pixel[0] +=  static_cast<unsigned char>(opacity * static_cast<float>(other.m_pixel[0])); 
    m_pixel[1] +=  static_cast<unsigned char>(opacity * static_cast<float>(other.m_pixel[1])); 
    m_pixel[2] +=  static_cast<unsigned char>(opacity * static_cast<float>(other.m_pixel[2])); 
    m_alpha += opacity;
    m_alpha = m_alpha > 1.f ? 1.f : m_alpha;
    
  }

  template<typename FloatType>
  inline void load_from_partial(const PartialImage<FloatType> &partial_image, const int &index)
  {
    
    m_pixel_id = static_cast<int>(partial_image.m_pixel_ids.GetPortalConstControl().Get(index));
    m_depth = static_cast<float>(partial_image.m_distances.GetPortalConstControl().Get(index));

    m_pixel[0] = static_cast<unsigned char>(partial_image.
                                  m_buffer.Buffer.GetPortalConstControl().Get(index*4+0) * 255);

    m_pixel[1] = static_cast<unsigned char>(partial_image.
                                  m_buffer.Buffer.GetPortalConstControl().Get(index*4+1) * 255);

    m_pixel[2] = static_cast<unsigned char>(partial_image.
                                  m_buffer.Buffer.GetPortalConstControl().Get(index*4+2) * 255);

    m_alpha = static_cast<float>(partial_image.
                                  m_buffer.Buffer.GetPortalConstControl().Get(index*4+3));
  }
  
  template<typename FloatType>
  inline void store_into_partial(PartialImage<FloatType> &output, const int &index)
  {
    const FloatType inverse = 1.f / 255.f;
    output.m_pixel_ids.GetPortalControl().Set(index, m_pixel_id ); 
    output.m_distances.GetPortalControl().Set(index, m_depth ); 
    const int starting_index = index * 4;
    output.m_buffer.Buffer.GetPortalControl().Set(starting_index + 0, static_cast<FloatType>(m_pixel[0])*inverse);
    output.m_buffer.Buffer.GetPortalControl().Set(starting_index + 1, static_cast<FloatType>(m_pixel[1])*inverse);
    output.m_buffer.Buffer.GetPortalControl().Set(starting_index + 2, static_cast<FloatType>(m_pixel[2])*inverse);
    output.m_buffer.Buffer.GetPortalControl().Set(starting_index + 3, static_cast<FloatType>(m_alpha));
  }

  static void composite_default_background(std::vector<VolumePartial> &partials)
  {
    VolumePartial bg_color;
    bg_color.m_pixel[0] = 255;
    bg_color.m_pixel[1] = 255;
    bg_color.m_pixel[2] = 255;
    bg_color.m_alpha = 1.f;
    //
    // Gather the unique pixels into the output
    //
    const int total_pixels = static_cast<int>(partials.size());
    #pragma omp parallel for 
    for(int i = 0; i < total_pixels; ++i)
    { 
      partials[i].blend(bg_color);
    }

  }
 
};

} // namespace
#endif
