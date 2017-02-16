#ifndef rover_loggin_h
#define rover_loggin_h

#include <fstream>

namespace rover {

class Logger 
{
public:
  ~Logger();
  static Logger *get_instance();
  void write(const int level, const std::string &message, const char *file, int line);
  std::ofstream & get_stream();
protected:
  Logger();
  Logger(Logger const &);
  std::ofstream m_stream;
  static class Logger* m_instance;
};


#define ROVER_INFO(msg) rover::Logger::get_instance()->get_stream() <<"<Info>\n" \
  <<"  message: "<< msg <<"\n  file: " <<__FILE__<<"\n  line:  "<<__LINE__<<std::endl;
#define ROVER_WARN(msg) rover::Logger::get_instance()->get_stream() <<"<Warn>\n" \
  <<"  message: "<< msg <<"\n  file: " <<__FILE__<<"\n  line:  "<<__LINE__<<std::endl;
#define ROVER_ERROR(msg) rover::Logger::get_instance()->get_stream() <<"<Error>\n" \
  <<"  message: "<< msg <<"\n  file: " <<__FILE__<<"\n  line:  "<<__LINE__<<std::endl;

} // namespace rover

#endif
