#ifndef rover_exceptions_h
#define rover_exceptions_h

#include <stdexcept>
#include <string>

namespace rover {

class RoverException : public std::runtime_error
{
public:
  RoverException(std::string message) : std::runtime_error(message.c_str()) {}
};


} // namespace rover

#endif
