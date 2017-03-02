#ifndef rover_volume_block_h
#define rover_volume_block_h

#include <utils/rover_logging.hpp>

#ifdef PARALLEL
#include <diy/master.hpp>
#endif
namespace {

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

    const float one_minus = 1.f - m_alpha;
    m_pixel[0] +=  static_cast<unsigned char>(one_minus * static_cast<float>(other.m_pixel[0])); 
    m_pixel[1] +=  static_cast<unsigned char>(one_minus * static_cast<float>(other.m_pixel[1])); 
    m_pixel[2] +=  static_cast<unsigned char>(one_minus * static_cast<float>(other.m_pixel[2])); 
    m_alpha += one_minus * other.m_alpha;
  }
};

#ifdef PARALLEL
struct VolumeBlock
{
  typedef diy::DiscreteBounds Bounds;

  std::vector<VolumePartial>   &m_partials;

  VolumeBlock(std::vector<VolumePartial> &partials)
    : m_partials(partials)
  {}
 /* 
  static void* create()
  {
    return new VolumeBlock;
  }

  static void destroy(void* volume_block)
  {
    delete static_cast<VolumeBlock*>(volume_block);
  }
    
  static void save(const void *volume_block, diy::BinaryBuffer &buffer)
  {
    const VolumeBlock *block = static_cast<const VolumeBlock*>(volume_block);
    diy::save(buffer, block->m_partials);
  }

  static void load(const void *volume_block, diy::BinaryBuffer &buffer)
  {
    const VolumeBlock *block = static_cast<const VolumeBlock*>(volume_block);
    diy::load(buffer, block->m_partials);
  }
*/
};

struct AddVolumeBlock
{
  std::vector<VolumePartial> &m_partials;
  const diy::Master &m_master;
  AddVolumeBlock(diy::Master &master,std::vector<VolumePartial> &partials)
    : m_master(master), m_partials(partials)
  {}
  template<typename BoundsType, typename LinkType>                 
  void operator()(int gid,
                  const BoundsType &local_bounds,
                  const BoundsType &local_with_ghost_bounds,
                  const BoundsType &domain_bounds,
                  const LinkType &link) const
  {
    VolumeBlock *volume_block = new VolumeBlock(m_partials);
    LinkType *rg_link = new LinkType(link);
    diy::Master& master = const_cast<diy::Master&>(m_master);
    int lid = master.add(gid, volume_block, rg_link);
  }
}; 

#endif

} // namespace
#endif
