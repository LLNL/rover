#include <utils/rover_logging.hpp>
#include <rover_exceptions.hpp>
#include <iostream>
#include <sstream>

#ifdef PARALLEL
#include <mpi.h>
#endif

namespace rover {

Logger* Logger::m_instance  = NULL;

Logger::Logger()
{
  std::stringstream log_name;
  log_name<<"rover";
#ifdef PARALLEL
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  log_name<<"_"<<rank;
#endif
  log_name<<".log";
  m_stream.open(log_name.str().c_str(), std::ofstream::out);
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
    m_stream<<"<Info> \n";
  else if (level == 1)
    m_stream<<"<Warning> \n";
  else if (level == 2)
    m_stream<<"<Error> \n";
  m_stream<<"  message: "<<message<<" \n  file: "<<file<<" \n  line: "<<line<<"\n";
}

} // namespace rover

