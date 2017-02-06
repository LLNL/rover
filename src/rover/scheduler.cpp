#include <ray_generators/camera_generator.hpp>
#include <scheduler.hpp>
#include <vtkm_typedefs.hpp>
#include <utils/png_encoder.hpp>
#include <vtkm/rendering/CanvasRayTracer.h>
#include <rover_exceptions.hpp>
#include <assert.h>
#include <utils/rover_logging.hpp>

namespace rover {

template<typename FloatType>
Scheduler<FloatType>::Scheduler()
{
}

template<typename FloatType>
Scheduler<FloatType>::~Scheduler()
{
}

template<typename FloatType>
void
Scheduler<FloatType>::set_render_settings(const RenderSettings render_settings)
{
  //
  //  In the serial schedular, the only setting that matter are 
  //  m_render_mode and m_scattering_mode
  //
  m_render_settings = render_settings;
  // TODO: make copy constructor so the mesh stuctures are not rebuilt when moving from
  //       volume to energy and vice versa
  const int num_domains = static_cast<int>(m_domains.size());
  for(int i = 0; i < num_domains; ++i) m_domains[i].set_render_settings(m_render_settings);
}

template<typename FloatType>
void 
Scheduler<FloatType>::add_data_set(vtkmDataSet &dataset)
{
  Domain domain;
  domain.set_render_settings(m_render_settings);
  domain.set_data_set(dataset);
  m_domains.push_back(domain);
}

template<typename FloatType>
vtkmDataSet
Scheduler<FloatType>::get_data_set(const int &domain)
{
  return m_domains.at(domain).get_data_set();
}

template<typename FloatType>
RenderSettings
Scheduler<FloatType>::get_render_settings() const
{
  return m_render_settings;
}

template<typename FloatType>
FloatType *
Scheduler<FloatType>::get_color_buffer()
{
  if(m_render_settings.m_render_mode == energy)
  {
    throw RoverException("cannot call get_color_buffer while using energy mode");
  }
  ROVER_ERROR("This should not be called"); 
  return get_vtkm_ptr(m_partial_composites.at(100).m_buffer.Buffer);
}

//
// in the other schedulars this method will be far from trivial
//
template<typename FloatType>
void 
Scheduler<FloatType>::trace_rays()
{
  // TODO while (m_geerator.has_rays())
  ROVER_INFO("Tracing rays");
  vtkmRayTracing::Ray<FloatType> rays = m_ray_generator->get_rays();

  const int num_domains = static_cast<int>(m_domains.size());
  for(int i = 0; i < num_domains; ++i)
  {
    ROVER_INFO("Tracing domain "<<i);
    m_domains[i].trace(rays);
    PartialComposite<FloatType> partial_comp;
    partial_comp.m_pixel_ids = rays.PixelIdx;

    if(m_render_settings.m_render_mode == energy)
    {
      partial_comp.m_buffer = rays.Buffers.at(0);
      assert(partial_comp.m_buffer.GetNumChannels() > 0);
    }
    else
    {
      // TODO: add some conversion to uchar probably withing the "channel buffer"
      assert(rays.Buffers.at(0).GetNumChannels() == 4); 
      partial_comp.m_buffer = rays.Buffers.at(0);

      /*
      int height = 0;
      int width = 0;
      m_ray_generator->get_dims(height, width);
      vtkm::rendering::CanvasRayTracer canvas(width, height);
      vtkm::rendering::Camera cam;
      canvas.WriteToCanvas(rays, cam);
      canvas.SaveAs("test.pnm"); 
      */
    }
    m_partial_composites.push_back(partial_comp);
  }// for each domain

  if(num_domains > 1)
  {
    ROVER_ERROR("compositing not implemented");
    //TODO: composite
  }
  else if(num_domains == 1)
  {
    m_result = m_partial_composites[0];
  }
  else 
  {
    ROVER_ERROR("Invalid number of domains: "<<num_domains);
  }
  m_partial_composites.clear();
}

template<typename FloatType>
void Scheduler<FloatType>::save_result(std::string file_name) 
{
  int height = 0;
  int width = 0;
  int buffer_size = m_ray_generator->get_size();
  m_ray_generator->get_dims(height, width);
  assert( height > 0 );
  assert( width > 0 );
  ROVER_INFO("Saving file " << height << " "<<width);
  PNGEncoder encoder;

  if(m_render_settings.m_render_mode == energy)
  {
    const int num_channels = m_result.m_buffer.GetNumChannels();
    ROVER_INFO("Saving "<<num_channels<<" channels ");
    for(int i = 0; i < num_channels; ++i)
    {
      std::stringstream sstream;
      sstream<<file_name<<"_"<<i<<".png";
      vtkmRayTracing::ChannelBuffer<FloatType> channel = m_result.m_buffer.GetChannel( i );
      const bool invert = true;
      channel.Normalize(invert);

      vtkmRayTracing::ChannelBuffer<FloatType>  expand = channel.ExpandBuffer(m_result.m_pixel_ids, buffer_size);
      FloatType * buffer 
        = get_vtkm_ptr(expand.Buffer);
      encoder.EncodeChannel(buffer, width, height);
      encoder.Save(sstream.str());
    }
  }
  else
  {
      
    vtkmRayTracing::ChannelBuffer<FloatType>  expand = m_result.m_buffer.ExpandBuffer(m_result.m_pixel_ids, buffer_size);
    FloatType * buffer 
      = get_vtkm_ptr(expand.Buffer);
    
    assert(m_result.m_buffer.GetNumChannels() == 4);
    encoder.Encode(buffer, width, height);
    encoder.Save(file_name + ".png");
  }
  
}

template<typename FloatType>
void Scheduler<FloatType>::set_ray_generator(RayGenerator<FloatType> *ray_generator)
{
  m_ray_generator = ray_generator;
}

template<typename FloatType>
void Scheduler<FloatType>::clear_data_sets()
{
  m_domains.clear();
}
//
// Explicit instantiation
template class Scheduler<vtkm::Float32>; template class Scheduler<vtkm::Float64>;
}; // namespace rover
