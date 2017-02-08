#ifndef rover_partial_composite_sorter_h
#define rover_partial_composite_sorter_h

#include <vector>

namespace rover {
  
template<typename Precision>
struct PartialComposite
{
  int                    m_pixel_id;
  Precision              m_depth; 
  std::vector<Precision> m_channels;
};

//
// Ideas for sorting:
//  1) sort based on pixel ids
//  2) double sort based on id / depth
//  3) find starting indexes of ranges that need compositing 
//     then parallel loop over and composite
//

} //namespace rover
#endif
