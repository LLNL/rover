#include <image.hpp>
#include <rover_exceptions.hpp>

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
  return static_cast<int>(m_channels.size());
}

template<typename FloatType>
bool
Image<FloatType>::has_channel(const int &channel_num) const
{
  if(channel_num < 0 || channel_num >= m_channels.size())
  {
    return false;
  }

  if(!m_valid_channels.at(channel_num))
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
Image<FloatType>::steal_channel(const int &channel_num)
{
  if(channel_num < 0 || channel_num >= m_channels.size())
  {
    throw RoverException("Rover Image: invalid channel number");
  }

  if(!m_valid_channels.at(channel_num))
  {
    throw RoverException("Rover Image: cannot steal a channel that has already been stolen");
  }
  m_channels[channel_num].SyncControlArray();
  FloatType *ptr = reinterpret_cast<FloatType*>(
      m_channels[channel_num].Internals->ControlArray.StealArray());
  return ptr;
}

template<typename FloatType>
void 
Image<FloatType>::init_from_partial(PartialImage<FloatType> &partial)
{
  m_channels.clear();
  m_valid_channels.clear();

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

    m_channels.push_back(expand.Buffer);
    m_valid_channels.push_back(true);

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
Image<FloatType>::get_channel(const int &channel_num)
{
  if(channel_num < 0 || channel_num >= m_channels.size())
  {
    throw RoverException("Rover Image: invalid channel number");
  }
  if(!m_valid_channels.at(channel_num))
  {
    throw RoverException("Rover Image: cannot get a channel that has already been stolen");
  }
  return m_channels[channel_num];
}

template<typename FloatType>
vtkm::cont::ArrayHandle<FloatType>
Image<FloatType>::flatten_channels()
{
  const int num_channels = this->get_num_channels();   
  for(int i = 0; i < num_channels; ++i)
  {
    if(!m_valid_channels.at(i))
    {
      throw RoverException("Rover Image: cannot flatten when channel has been stolen");
    }
  }
  HandleType res;
  const int size = m_width * m_height;
  res.Allocate(num_channels * size);
  auto output = res.GetPortalControl();
  for(int c = 0; c < num_channels; ++c)
  {
    auto channel = m_channels[c].GetPortalControl();
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
Image<FloatType>::normalize_channel(const int &channel_num)
{
  if(channel_num < 0 || channel_num >= m_channels.size())
  {
    throw RoverException("Rover Image: invalid channel number");
  }
  if(!m_valid_channels.at(channel_num))
  {
    throw RoverException("Rover Image: cannot normalize a channel that has already been stolen");
  }
  bool invert = false;
  normalize_handle(m_channels[channel_num], invert);
}
//
// Explicit instantiation
template class Image<vtkm::Float32>; 
template class Image<vtkm::Float64>;

} // namespace rover
