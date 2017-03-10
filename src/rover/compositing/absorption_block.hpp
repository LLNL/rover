#ifndef rover_absorption_block_h
#define rover_absorption_block_h

#include <utils/rover_logging.hpp>
#include <assert.h>
#ifdef PARALLEL
#include <diy/master.hpp>
#endif
namespace rover {

template<typename FloatType>
struct AbsorptionPartial
{
  int                    m_pixel_id;
  double                 m_depth; 
  std::vector<FloatType> m_bins;
  AbsorptionPartial()
    : m_pixel_id(0),
      m_depth(0.f)
  {

  }

  bool operator < (const AbsorptionPartial<FloatType> &other) const
  {
    //
    // In absorption only we can blend the same
    // pixel ids in any order
    //
    if(m_pixel_id != other.m_pixel_id) 
    {
      return m_pixel_id < other.m_pixel_id;
    }
  }

  inline void blend(const AbsorptionPartial<FloatType> &other)
  {
    const int num_bins = m_bins.size();
    assert(num_bins == other.m_bins.size());
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
    const int starting_index = index * num_bins;
    for(int i = 0; i < num_bins; ++i)
    {
      m_bins[i] = partial_image.m_buffer.Buffer.GetPortalConstControl().Get(starting_index + 0);
    }
  }
  
  inline void store_into_partial(PartialImage<FloatType> &output, const int &index)
  {
    const int num_bins = m_bins.size();
    output.m_pixel_ids.GetPortalControl().Set(index, m_pixel_id ); 
    output.m_distances.GetPortalControl().Set(index, m_depth ); 
    const int starting_index = num_bins * 4;
    for(int  i = 0; i < num_bins; ++i)
    {
      output.m_buffer.Buffer.GetPortalControl().Set(starting_index + i, m_bins[i]);
    } 
  }

  static void composite_default_background(std::vector<AbsorptionPartial<FloatType>> &partials)
  {

  }

};


typedef AbsorptionPartial<double> AbsorptionPartial64;

#ifdef PARALLEL
template<typename FloatType>
struct AbsorptionBlock
{
  typedef diy::DiscreteBounds Bounds;

  std::vector<AbsorptionPartial<FloatType>>   &m_partials;

  AbsorptionBlock(std::vector<AbsorptionPartial<FloatType>> &partials)
    : m_partials(partials)
  {}
};

template<typename FloatType>
struct AddAbsorptionBlock
{
  std::vector<AbsorptionPartial<FloatType>> &m_partials;
  const diy::Master &m_master;

  AddAbsorptionBlock(diy::Master &master, std::vector<AbsorptionPartial<FloatType>> &partials)
    : m_master(master), m_partials(partials)
  {}

  template<typename BoundsType, typename LinkType>                 
  void operator()(int gid,
                  const BoundsType &local_bounds,
                  const BoundsType &local_with_ghost_bounds,
                  const BoundsType &domain_bounds,
                  const LinkType &link) const
  {
    AbsorptionBlock<FloatType> *absorption_block = new AbsorptionBlock<FloatType>(m_partials);
    LinkType *rg_link = new LinkType(link);
    diy::Master& master = const_cast<diy::Master&>(m_master);
    int lid = master.add(gid, absorption_block, rg_link);
  }
}; 

#endif

} // namespace rover

#ifdef PARALLEL
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
#endif
