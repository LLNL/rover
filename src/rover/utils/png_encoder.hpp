#ifndef rover_png_encoder_h
#define rover_png_encoder_h

#include <string>
namespace rover {

class PNGEncoder
{
public:
  PNGEncoder();
  ~PNGEncoder();
  
  void           Encode(const unsigned char *rgba_in,
                        const int width,
                        const int height);
  void           Encode(const float *rgba_in,
                        const int width,
                        const int height);

  void           Encode(const double *rgba_in,
                        const int width,
                        const int height);

  void           Save(const std::string &filename);

  void          *PngBuffer();
  size_t         PngBufferSize();

  
  void           Cleanup();
  
private:
  unsigned char *m_buffer;
  size_t         m_buffer_size;
};

} // namespace rover

#endif
