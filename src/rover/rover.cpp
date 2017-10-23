#include <scheduler.hpp>
#include <rover.hpp>
#include <rover_exceptions.hpp>
#include <vtkm_typedefs.hpp>
#include <iostream>
#include <utils/rover_logging.hpp>
namespace rover {

class Rover::InternalsType 
{
public: 
  enum TracePrecision {ROVER_FLOAT, ROVER_DOUBLE};
protected:
  SchedulerBase            *m_scheduler;
  TracePrecision            m_precision;
#ifdef PARALLEL
  MPI_Comm                  m_comm_handle;
  int                       m_rank;
  int                       m_num_ranks;
#endif

  void reset_render_mode(RenderMode render_mode)
  {
  }

public: 
  InternalsType()
  {
    m_precision = ROVER_FLOAT;
    m_scheduler = new Scheduler<vtkm::Float32>();

#ifdef PARALLEL
    m_rank = 1;
    m_num_ranks = -1;
#endif
  }

  void add_data_set(vtkmDataSet &dataset)
  {
    ROVER_INFO("Adding data set");
    m_scheduler->add_data_set(dataset);
  }

  void set_render_settings(RenderSettings render_settings)
  {
    ROVER_INFO("set_render_settings");
    // TODO: make copy constructors to get the members like ray_generator
//#ifdef PARALLEL
    // logic to create the appropriate parallel scheduler
    //
    // ray tracing = dynamic scheduler, scattering | no_scattering
    // volume/engery = scattering + local_scope -> dynamic scheduler 
    //                 non_scattering + global_scope ->static scheduler
    //
    // Note: I wanted to allow for the case of scattering + global scope. This could 
    //       be benificial in the case where we may or may not scatter in a given 
    //       domain. Thus, avoid waiting for the ray to emerge or throw out the results
//#else
     //if(render_settings compared to old means new schedular)
     //if(m_scheduler == NULL) delete m_scheduler;
     //m_scheduler = new Scheduler<FloatType>();
     m_scheduler->set_render_settings(render_settings);
//#endif
   }

  void set_ray_generator(RayGenerator *ray_generator)
  {
    m_scheduler->set_ray_generator(ray_generator); 
  }

  void clear_data_sets()
  {
    m_scheduler->clear_data_sets(); 
  }

  ~InternalsType()
  {
    if(m_scheduler) delete m_scheduler;
  }
  
  void set_background(const std::vector<vtkm::Float32> &background)
  {
    m_scheduler->set_background(background);
  }

  void set_background(const std::vector<vtkm::Float64> &background)
  {
    m_scheduler->set_background(background);
  }

  void save_png(const std::string &file_name)
  {
#ifdef PARALLEL
    if(m_rank != 0)
    {
      return;
    }
#endif
    m_scheduler->save_result(file_name);
  }

  void execute()
  {
#ifdef PARALLEL
    //
    // Check to see if we have been initialized
    //
    if(m_rank == -1)
    {
      ROVER_ERROR("Execute call with MPI enbaled, but never initialized with comm handle");
    }

    m_scheduler->set_comm_handle(m_comm_handle);
#endif
    m_scheduler->trace_rays();
  }
#ifdef PARALLEL
  void set_comm_handle(MPI_Comm comm_handle)
  {
    m_comm_handle = comm_handle;
    MPI_Comm_rank(m_comm_handle, &m_rank);
    if(m_rank == 0)
    {
      MPI_Comm_size(m_comm_handle, &m_num_ranks);
      ROVER_INFO("MPI Comm size : "<<m_num_ranks);
    }
  }
#endif
  void get_result(Image<vtkm::Float32> &image)
  {
    m_scheduler->get_result(image);
  }

  void get_result(Image<vtkm::Float64> &image)
  {
    m_scheduler->get_result(image);
  }

  void set_tracer_precision32()
  {
    if(m_precision == ROVER_DOUBLE)
    {
      std::vector<Domain> domains = m_scheduler->get_domains(); 
      delete m_scheduler;
      m_scheduler = new Scheduler<vtkm::Float32>();
    } 
  }

  void set_tracer_precision64()
  {
    if(m_precision == ROVER_FLOAT)
    {
      std::vector<Domain> domains = m_scheduler->get_domains(); 
      delete m_scheduler;
      m_scheduler = new Scheduler<vtkm::Float64>();
    } 
  }

}; //Internals Type

Rover::Rover()
  : m_internals( new InternalsType )
{

}

Rover::~Rover()
{
  
}

#ifdef PARALLEL
void
Rover::init(MPI_Comm comm_handle)
{
  this->m_internals->set_comm_handle(comm_handle);
}
#else
void
Rover::init()
{
  // initialize
}
#endif

void
Rover::finalize()
{
  DataLogger::GetInstance()->WriteLog();
}

void
Rover::add_data_set(vtkmDataSet &dataset)
{
  m_internals->add_data_set(dataset); 
}

void
Rover::set_render_settings(RenderSettings render_settings)
{
  m_internals->set_render_settings(render_settings);
}

void
Rover::clear_data_sets()
{
  m_internals->clear_data_sets();
}

void
Rover::set_ray_generator(RayGenerator *ray_generator)
{
  if(ray_generator == nullptr)
  {
    throw RoverException("Ray generator cannot  be null");    
  }
  m_internals->set_ray_generator(ray_generator);
}

void
Rover::execute()
{
  m_internals->execute(); 
}

template<typename T> 
bool
is_float(T ) { } 

template<> 
bool
is_float<vtkm::Float32>(vtkm::Float32 )
{
  return true;
}

template<> 
bool
is_float<vtkm::Float64>(vtkm::Float64)
{
  return false;
}

void
Rover::about()
{
  std::cout<<"rover version: xx.xx.xx\n";
  
  //if(is_float(FloatType())) std::cout<<"Single precision\n";
  //else std::cout<<"Double precision\n";
  std::cout<<"Other important information\n"; 
  std::cout<<"                                 *@@                                    \n";    
  std::cout<<"       @@@@@@@@@@@@@@,          @@&@@              %@@@                 \n";    
  std::cout<<"       @@@@@%  #@@@@@,         &@* @@,              @@@#                \n";    
  std::cout<<"       @@@@ @    @@@@,         @@   @@             @@ .@@               \n";    
  std::cout<<"       @@@@@    #@@@@,        .@&    @@           @@(   @@@             \n";    
  std::cout<<"       @@@@@@@@@@@@@@,         @@    %@&         .@@      @@            \n";    
  std::cout<<"       &&&&@@&&@@&&&&.     @@@@@@%    @@         @@        @@@          \n";    
  std::cout<<"           %@, @@          @@   &@@    @@       @@           @@.        \n";    
  std::cout<<"           %@, @@          @@     @@@  ,@@     *@&            (@@       \n";    
  std::cout<<"           %@, @@          @@       ,@@@@@     @@               @@(     \n";    
  std::cout<<"          *&@(*@@*         @@                 @@                 (@@@#  \n";    
  std::cout<<"          @@@@@@@@         @@                @@%                  @@.@@ \n";    
  std::cout<<"          @@    @@         @@                @@                   @@  @@\n";    
  std::cout<<"          @@    @@        @@@@              @@                    @@ @@.\n";    
  std::cout<<"          @@    @@        @@@@             @@                     @@@%  \n";    
  std::cout<<"          @@    @@        @@@@         /@@@@@@@@@@@&                    \n";    
  std::cout<<"   ,,,,,,,@@,,,,@@,,,,,,,,@@@@,,,,,,,,,(@#,,,,,,,,@@@,,,,,,,,,,,,,,.    \n";    
  std::cout<<"  @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@    \n";    
  std::cout<<"  @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@    \n";    
  std::cout<<"     /@@*                                                     @@%       \n";    
  std::cout<<"       .@@*                                                .@@%         \n";    
  std::cout<<"         *@@.                                             @@#           \n";    
  std::cout<<"           ,@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@(             \n";    
  std::cout<<"           #@(                                 @@                       \n";    
  std::cout<<"           @@                     /@@@@@@@@@@@@@@@@@@@@@@@@@@@          \n";    
  std::cout<<"          @@                      /@/                       @@          \n";    
  std::cout<<"      @@@@@@@@(                &@@@@@@@&                #@@@@@@@@       \n";    
  std::cout<<"   &@@@@@@@@@@@@@           /@@@@@@@@@@@@@            @@@@@@@@@@@@@%    \n";    
  std::cout<<"  @@@@@   @  #@@@@#        @@@@@   @  ,@@@@@        %@@@@(  @   @@@@@   \n";    
  std::cout<<" @@@@,@/,@@@ @%@@@@,      %@@@%@( @@@ %@@@@@%      ,@@@@%@ @@@,/@#@@@@  \n";    
  std::cout<<" @@@@  .@& @@  *@@@@      @@@@  .@@ @@   @@@@      @@@@   @@ &@.  @@@@  \n";    
  std::cout<<" @@@@ #@@@@@@@.@@@@(      @@@@.*@@@@@@@*(@@@@      (@@@# @@@@@@@#.@@@@  \n";    
  std::cout<<"  @@@@*   @   @@@@@        @@@@&   @   &@@@@        @@@@@   @   /@@@@   \n";    
  std::cout<<"   @@@@@@@@@@@@@@#          @@@@@@@@@@@@@@@          @@@@@@@@@@@@@@@    \n";    
  std::cout<<"     @@@@@@@@@@*              &@@@@@@@@@#              #@@@@@@@@@@      \n";    
                                                                             
}

void 
Rover::set_background(const std::vector<vtkm::Float64> &background)
{
  m_internals->set_background(background);
} 

void 
Rover::set_background(const std::vector<vtkm::Float32> &background)
{
  m_internals->set_background(background);
} 

void
Rover::save_png(const std::string &file_name)
{
  m_internals->save_png(file_name);
}

void
Rover::get_result(Image<vtkm::Float32> &image)
{
  m_internals->get_result(image);
}

void
Rover::get_result(Image<vtkm::Float64> &image)
{
  m_internals->get_result(image);
}

void 
Rover::set_tracer_precision32()
{
  m_internals->set_tracer_precision32();
}

void 
Rover::set_tracer_precision64()
{
  m_internals->set_tracer_precision64();
}

}; //namespace rover

