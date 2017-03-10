#ifndef rover_volume_block_h
#define rover_volume_block_h

#include <utils/rover_logging.hpp>

#ifdef PARALLEL
#include <diy/master.hpp>
#endif
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
    const float one_minus = 1.f - m_alpha;
    m_pixel[0] +=  static_cast<unsigned char>(one_minus * static_cast<float>(other.m_pixel[0])); 
    m_pixel[1] +=  static_cast<unsigned char>(one_minus * static_cast<float>(other.m_pixel[1])); 
    m_pixel[2] +=  static_cast<unsigned char>(one_minus * static_cast<float>(other.m_pixel[2])); 
    m_alpha += one_minus * other.m_alpha;
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


#ifdef PARALLEL
struct VolumeBlock
{
  typedef diy::DiscreteBounds Bounds;
  typedef VolumePartial       PartialType;
  std::vector<VolumePartial>  &m_partials;

  VolumeBlock(std::vector<VolumePartial> &partials)
    : m_partials(partials)
  {}
};

template<typename BlockType>
struct AddBlock
{
  typedef typename BlockType::PartialType PartialType;
  typedef BlockType                       Block;
  std::vector<PartialType> &m_partials;
  const diy::Master &m_master;

  AddBlock(diy::Master &master,std::vector<PartialType> &partials)
    : m_master(master), m_partials(partials)
  {}
  template<typename BoundsType, typename LinkType>                 
  void operator()(int gid,
                  const BoundsType &local_bounds,
                  const BoundsType &local_with_ghost_bounds,
                  const BoundsType &domain_bounds,
                  const LinkType &link) const
  {
    Block *block = new Block(m_partials);
    LinkType *rg_link = new LinkType(link);
    diy::Master& master = const_cast<diy::Master&>(m_master);
    int lid = master.add(gid, block, rg_link);
  }
}; 

#endif

} // namespace
#endif
