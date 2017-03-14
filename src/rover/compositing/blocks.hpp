#ifndef rover_blocks_h
#define rover_blocks_h

#include <diy/master.hpp>

#include <compositing/absorption_partial.hpp>
#include <compositing/volume_partial.hpp>

namespace rover {

//--------------------------------------Volume Block Structure-----------------------------------
struct VolumeBlock
{
  typedef diy::DiscreteBounds Bounds;
  typedef VolumePartial       PartialType;
  std::vector<VolumePartial>  &m_partials;
  VolumeBlock(std::vector<VolumePartial> &partials)
    : m_partials(partials)
  {}
};


//--------------------------------------Absorption Block Structure------------------------------
template<typename FloatType>
struct AbsorptionBlock
{
  typedef diy::DiscreteBounds Bounds;
  typedef AbsorptionPartial<FloatType> PartialType; 
  std::vector<AbsorptionPartial<FloatType>>   &m_partials;

  AbsorptionBlock(std::vector<AbsorptionPartial<FloatType>> &partials)
    : m_partials(partials)
  {}
};

//--------------------------------------Add Block Template-----------------------------------
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

} //namespace rover

//-------------------------------Serialization Specializations--------------------------------
namespace diy {

template<>
struct Serialization<rover::AbsorptionPartial<double>>
{

  static void save(BinaryBuffer& bb, const rover::AbsorptionPartial<double> &partial)
  { 
    diy::save(bb, partial.m_bins); 
    diy::save(bb, partial.m_pixel_id);
    diy::save(bb, partial.m_depth);
  }

  static void load(BinaryBuffer& bb, rover::AbsorptionPartial<double> &partial)
  { 
    diy::load(bb, partial.m_bins); 
    diy::load(bb, partial.m_pixel_id);
    diy::load(bb, partial.m_depth);
  }
};

template<>
struct Serialization<rover::AbsorptionPartial<float>>
{

  static void save(BinaryBuffer& bb, const rover::AbsorptionPartial<float> &partial)
  { 
    diy::save(bb, partial.m_bins); 
    diy::save(bb, partial.m_pixel_id);
    diy::save(bb, partial.m_depth);
  }

  static void load(BinaryBuffer& bb, rover::AbsorptionPartial<float> &partial)
  { 
    diy::load(bb, partial.m_bins); 
    diy::load(bb, partial.m_pixel_id);
    diy::load(bb, partial.m_depth);
  }
};

} // namespace diy

#endif
