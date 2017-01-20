#include <utils/rover_logging.hpp>
#include <rover_exceptions.hpp>
#include <iostream>

namespace rover {

Logger* Logger::m_instance  = NULL;

Logger::Logger()
{
  m_stream.open("rover.log", std::ofstream::out);
  if(!m_stream.is_open())
    std::cout<<"Warning: could not open the rover log file\n";
}

Logger::~Logger()
{
  if(m_stream.is_open())
    m_stream.close();
}

Logger* Logger::get_instance()
{
  if(m_instance == NULL)
    m_instance =  new Logger();
  return m_instance;
}

std::ofstream& Logger::get_stream() 
{
  return m_stream;
}

void
Logger::write(const int level, const std::string &message, const char *file, int line)
{
  if(level == 0)
    m_stream<<"<Info> ";
  else if (level == 1)
    m_stream<<"<Warning> ";
  else if (level == 2)
    m_stream<<"<Error> ";
  m_stream<<message<<" "<<file<<" "<<line<<std::endl;
}

} // namespace rover

