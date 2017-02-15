#ifndef rover_compositor_h
#define rover_compositor_h
#include <rover_types.hpp>
namespace rover {


class VolumeCompositor
{
public:
  VolumeCompositor();
  ~VolumeCompositor();

  template<typename FloatType> 
  PartialImage<FloatType> composite(std::vector<PartialImage<FloatType>> &partial_images);
protected:
  struct PartialComposite;  
};

}; // namespace rover
#endif
