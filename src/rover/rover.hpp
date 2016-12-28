#ifndef rover_h
#define rover_h

#include <rover_exports.h>

namespace rover {

class Rover 
{
public:
  Rover();
  ~Rover();
  
  void Init();
  void Finalize();

private:
   // Forward declaration of the engine
   class Engine;
   Engine *engine;
}; // class strawman 

}; // namespace rover

#endif
