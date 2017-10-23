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
  
  FloatType * steal_intensity(const int &channel_num);
  HandleType  get_intensity(const int &channel_num);
  FloatType * steal_optical_depth(const int &channel_num);
  HandleType  get_optical_depth(const int &channel_num);
  int get_num_channels() const;
  bool has_path_lengths() const;
  HandleType get_path_lengths();
  FloatType* steal_path_lengths();
  bool has_intensity(const int &channel_num) const;
  bool has_optical_depth(const int &channel_num) const;
  void normalize_intensity(const int &channel_num);
  void normalize_optical_depth(const int &channel_num);
  void normalize_paths();
  void operator=(PartialImage<FloatType> partial);
  template<typename O> void operator=(Image<O> &other);
  HandleType flatten_intensities();
  HandleType flatten_optical_depths();
  int get_size();
  template<typename T, 
           typename O> friend void init_from_image(Image<T> &left, 
                                                   Image<O> &right);
  
protected:
  int                                      m_height;
  int                                      m_width;
  bool                                     m_has_path_lengths;
  std::vector<HandleType>                  m_intensities;
  std::vector<HandleType>                  m_optical_depths;
  std::vector<bool>                        m_valid_intensities;
  std::vector<bool>                        m_valid_optical_depths;
  HandleType                               m_path_lengths; 

  void init_from_partial(PartialImage<FloatType> &);
  void normalize_handle(HandleType &, bool);
};
} // namespace rover
#endif
