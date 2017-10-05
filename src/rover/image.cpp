#include <image.hpp>
#include <rover_exceptions.hpp>
#include <utils/rover_logging.hpp>

#include <vtkm/cont/Field.h>

namespace rover
{

template<typename FloatType>
void 
Image<FloatType>::normalize_handle(vtkm::cont::ArrayHandle<FloatType> &handle, bool invert)
{

  vtkm::cont::Field as_field("name meaningless", 
                             vtkm::cont::Field::ASSOC_POINTS,
                             handle);
  vtkm::Range range;
  as_field.GetRange(&range);
  FloatType min_scalar = static_cast<FloatType>(range.Min);
  FloatType max_scalar = static_cast<FloatType>(range.Max);
  printf("min %3.15f\n", min_scalar);
  printf("max %3.15f\n", max_scalar);
  FloatType inv_delta; 
  inv_delta = min_scalar == max_scalar ? 1.f : 1.f / (max_scalar - min_scalar); 
  auto portal = handle.GetPortalControl();
  const int size = m_width * m_height;
  #pragma omp parallel for
  for(int i = 0; i < size; ++i)
  {
    FloatType val = portal.Get(i);
    val = (val - min_scalar) * inv_delta;
    if(invert) val = 1.f - val;
    portal.Set(i, val);
  }
}

template<typename FloatType>
Image<FloatType>::Image()
  : m_width(0),
    m_height(0)
{
  
}

template<typename FloatType>
Image<FloatType>::Image(PartialImage<FloatType> &partial)
{
  this->init_from_partial(partial);  
}

template<typename FloatType>
void 
Image<FloatType>::operator=(PartialImage<FloatType> partial)
{ 
  init_from_partial(partial);
}

template<typename FloatType>
int 
Image<FloatType>::get_num_channels() const
{
  return static_cast<int>(m_intensities.size());
}

template<typename FloatType>
bool
Image<FloatType>::has_intensity(const int &channel_num) const
{
  if(channel_num < 0 || channel_num >= m_intensities.size())
  {
    return false;
  }

  if(!m_valid_intensities.at(channel_num))
  {
    return false;
  }

  return true;
}

template<typename FloatType>
bool
Image<FloatType>::has_optical_depth(const int &channel_num) const
{
  if(channel_num < 0 || channel_num >= m_optical_depths.size())
  {
    return false;
  }

  if(!m_valid_optical_depths.at(channel_num))
  {
    return false;
  }

  return true;
}

template<typename FloatType>
vtkm::cont::ArrayHandle<FloatType>
Image<FloatType>::get_path_lengths()
{
  if(!m_has_path_lengths)
  {
    throw RoverException("Rover Image: cannot get paths. They dont exist or have already been stolen.");
  }

  return m_path_lengths;
}

template<typename FloatType>
void
Image<FloatType>::normalize_paths()
{
  if(!m_has_path_lengths)
  {
    throw RoverException("Rover Image: cannot get paths. They dont exist or have already been stolen.");
  }
  bool invert = false;
  normalize_handle(m_path_lengths, false);
}

template<typename FloatType>
FloatType * 
Image<FloatType>::steal_path_lengths()
{
  if(!m_has_path_lengths)
  {
    throw RoverException("Rover Image: cannot steal paths. They dont exist or have already been stolen.");
  }

  m_path_lengths.SyncControlArray();
  FloatType *ptr = reinterpret_cast<FloatType*>(
      m_path_lengths.Internals->ControlArray.StealArray());
  m_has_path_lengths = false;
  return ptr;
}

template<typename FloatType>
bool
Image<FloatType>::has_path_lengths() const
{
  return m_has_path_lengths;
}

template<typename FloatType>
FloatType * 
Image<FloatType>::steal_intensity(const int &channel_num)
{
  if(channel_num < 0 || channel_num >= m_intensities.size())
  {
    throw RoverException("Rover Image: invalid channel number");
  }

  if(!m_valid_intensities.at(channel_num))
  {
    throw RoverException("Rover Image: cannot steal an instensity channel that has already been stolen");
  }
  m_intensities[channel_num].SyncControlArray();
  FloatType *ptr = reinterpret_cast<FloatType*>(
      m_intensities[channel_num].Internals->ControlArray.StealArray());
  return ptr;
}

template<typename FloatType>
FloatType * 
Image<FloatType>::steal_optical_depth(const int &channel_num)
{
  if(channel_num < 0 || channel_num >= m_intensities.size())
  {
    throw RoverException("Rover Image: invalid channel number");
  }

  if(!m_valid_optical_depths.at(channel_num))
  {
    throw RoverException("Rover Image: cannot steal an optical depth channel that has already been stolen");
  }
  m_optical_depths[channel_num].SyncControlArray();
  FloatType *ptr = reinterpret_cast<FloatType*>(
      m_optical_depths[channel_num].Internals->ControlArray.StealArray());
  return ptr;
}

template<typename FloatType>
void 
Image<FloatType>::init_from_partial(PartialImage<FloatType> &partial)
{
  m_intensities.clear();
  m_optical_depths.clear();
  m_valid_intensities.clear();
  m_valid_optical_depths.clear();

  m_height = partial.m_height;
  m_width  = partial.m_width;
  assert(m_width > 0);
  assert(m_height > 0);
  m_has_path_lengths = partial.m_path_lengths.GetNumberOfValues() != 0;
  if(partial.m_buffer.GetSize() == 0)
  {
    return;
  }
  const int num_channels = partial.m_buffer.GetNumChannels();
  for(int i = 0; i < num_channels; ++i)
  {
    vtkmRayTracing::ChannelBuffer<FloatType> channel = partial.m_buffer.GetChannel( i );
    const FloatType default_value = partial.m_source_sig[i];;
    const int channel_size = m_height * m_width;
    vtkmRayTracing::ChannelBuffer<FloatType>  expand;
    expand = channel.ExpandBuffer(partial.m_pixel_ids, 
                                  channel_size, 
                                  default_value);

    m_optical_depths.push_back(expand.Buffer);
    m_valid_optical_depths.push_back(true);

  }

  for(int i = 0; i < num_channels; ++i)
  {
    vtkmRayTracing::ChannelBuffer<FloatType> channel = partial.m_intensities.GetChannel( i );
    const FloatType default_value = partial.m_source_sig[i];;
    const int channel_size = m_height * m_width;
    vtkmRayTracing::ChannelBuffer<FloatType>  expand;
    expand = channel.ExpandBuffer(partial.m_pixel_ids, 
                                  channel_size, 
                                  default_value);

    m_intensities.push_back(expand.Buffer);
    m_valid_intensities.push_back(true);

  }

  if(m_has_path_lengths)
  {
    const int size = m_width * m_height;
    m_path_lengths.Allocate(size);
    auto portal = m_path_lengths.GetPortalControl();
    #pragma omp parallel for
    for(int i = 0; i < size; ++i)
    {
      portal.Set(i, 0.0f);
    }
    const int num_ids = static_cast<int>(partial.m_pixel_ids.GetNumberOfValues());
    auto id_portal = partial.m_pixel_ids.GetPortalControl(); 
    auto path_portal = partial.m_path_lengths.GetPortalControl(); 
    #pragma omp parallel for
    for(int i = 0; i < num_ids; ++i)
    {
      const int index = id_portal.Get(i);
      portal.Set(index, path_portal.Get(i));
    }
  }  
}

template<typename FloatType>
vtkm::cont::ArrayHandle<FloatType>
Image<FloatType>::get_intensity(const int &channel_num)
{
  if(channel_num < 0 || channel_num >= m_intensities.size())
  {
    throw RoverException("Rover Image: invalid channel number");
  }
  if(!m_valid_intensities.at(channel_num))
  {
    throw RoverException("Rover Image: cannot get an intensity that has already been stolen");
  }
  return m_intensities[channel_num];
}

template<typename FloatType>
vtkm::cont::ArrayHandle<FloatType>
Image<FloatType>::get_optical_depth(const int &channel_num)
{
  if(channel_num < 0 || channel_num >= m_optical_depths.size())
  {
    throw RoverException("Rover Image: invalid channel number");
  }
  if(!m_valid_optical_depths.at(channel_num))
  {
    throw RoverException("Rover Image: cannot get an optical depth that has already been stolen");
  }
  return m_optical_depths[channel_num];
}

template<typename FloatType>
vtkm::cont::ArrayHandle<FloatType>
Image<FloatType>::flatten_intensities()
{
  const int num_channels = this->get_num_channels();   
  for(int i = 0; i < num_channels; ++i)
  {
    if(!m_valid_intensities.at(i))
    {
      throw RoverException("Rover Image: cannot flatten intensities when channel has been stolen");
    }
  }
  HandleType res;
  const int size = m_width * m_height;
  res.Allocate(num_channels * size);
  auto output = res.GetPortalControl();
  for(int c = 0; c < num_channels; ++c)
  {
    auto channel = m_intensities[c].GetPortalControl();
    #pragma omp parallel for
    for(int i = 0; i < size; ++i)
    {
      output.Set( i * num_channels + c, channel.Get(i));
    }
  }
  return res;
}

template<typename FloatType>
vtkm::cont::ArrayHandle<FloatType>
Image<FloatType>::flatten_optical_depths()
{
  const int num_channels = this->get_num_channels();   
  for(int i = 0; i < num_channels; ++i)
  {
    if(!m_valid_optical_depths.at(i))
    {
      throw RoverException("Rover Image: cannot flatten optical depths when channel has been stolen");
    }
  }
  HandleType res;
  const int size = m_width * m_height;
  res.Allocate(num_channels * size);
  auto output = res.GetPortalControl();
  for(int c = 0; c < num_channels; ++c)
  {
    auto channel = m_optical_depths[c].GetPortalControl();
    #pragma omp parallel for
    for(int i = 0; i < size; ++i)
    {
      output.Set( i * num_channels + c, channel.Get(i));
    }
  }
  return res;
}

template<typename FloatType>
int
Image<FloatType>::get_size()
{
  return  m_width * m_height;
}

template<typename FloatType>
void
Image<FloatType>::normalize_intensity(const int &channel_num)
{
  if(channel_num < 0 || channel_num >= m_intensities.size())
  {
    throw RoverException("Rover Image: invalid channel number");
  }
  if(!m_valid_intensities.at(channel_num))
  {
    throw RoverException("Rover Image: cannot normalize an intensity channel that has already been stolen");
  }
  bool invert = false;
  normalize_handle(m_intensities[channel_num], invert);
}

template<typename FloatType>
void
Image<FloatType>::normalize_optical_depth(const int &channel_num)
{
  if(channel_num < 0 || channel_num >= m_optical_depths.size())
  {
    throw RoverException("Rover Image: invalid channel number");
  }
  if(!m_valid_optical_depths.at(channel_num))
  {
    throw RoverException("Rover Image: cannot normalize an optical depth channel that has already been stolen");
  }
  bool invert = false;
  normalize_handle(m_optical_depths[channel_num], invert);
}
//
// Explicit instantiation
template class Image<vtkm::Float32>; 
template class Image<vtkm::Float64>;

} // namespace rover
