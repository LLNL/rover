#ifndef rover_image_h
#define rover_image_h

#include <vector>

#include <vtkm/cont/ArrayHandle.h>

#include <rover_types.hpp>

namespace rover
{
  
template<typename FloatType>
class Image
{
public:
  typedef vtkm::cont::ArrayHandle<FloatType> HandleType;
  
  Image();
  Image(PartialImage<FloatType> &partial);
  
  FloatType * steal_channel(const int &channel_num);
  HandleType  get_channel(const int &channel_num);
  int get_num_channels() const;
  bool has_path_lengths() const;
  HandleType get_path_lengths();
  FloatType* steal_path_lengths();
  bool has_channel(const int &channel_num) const;
  void normalize_channel(const int &channel_num);
  void normalize_paths();
  void operator=(PartialImage<FloatType> partial);
  HandleType flatten_channels();
protected:
  int                                      m_height;
  int                                      m_width;
  bool                                     m_has_path_lengths;
  std::vector<HandleType>                  m_channels;
  std::vector<bool>                        m_valid_channels;
  HandleType                               m_path_lengths; 

  void init_from_partial(PartialImage<FloatType> &);
  void normalize_handle(HandleType &, bool);
};
} // namespace rover
#endif
