#ifndef rover_absorption_redistribute_h
#define rover_absorption_redistribute_h

#include <diy/assigner.hpp>
#include <diy/decomposition.hpp>
#include <diy/master.hpp>
#include <diy/reduce-operations.hpp>
#include <compositing/absorption_block.hpp>
#include <utils/rover_logging.hpp>
namespace rover{
//
// Redistributes partial composites to the ranks that owns 
// that sectoon of the image. Currently, the domain is decomposed
// in 1-D from min_pixel to max_pixel.
//
namespace {
struct Redistribute
{
  const diy::RegularDecomposer<diy::DiscreteBounds> &m_decomposer;

  Redistribute(const diy::RegularDecomposer<diy::DiscreteBounds> &decomposer)
    : m_decomposer(decomposer)
  {}

  void operator()(void *v_block, const diy::ReduceProxy &proxy) const
  {
    AbsorptionBlock *volume_block = static_cast<AbsorptionBlock*>(v_block);
    //
    // first round we have no incoming. Take the partials we have
    // and sent them to to the right rank
    //
    if(proxy.in_link().size() == 0)
    {
      const int size = volume_block->m_partials.size(); 
      ROVER_INFO("Processing partials block of size "<<size);

      for(int i = 0; i < size; ++i)
      {
        diy::Point<int,DIY_MAX_DIM> point;
        point[0] = volume_block->m_partials[i].m_pixel_id;
        int dest_gid = m_decomposer.point_to_gid(point);
        diy::BlockID dest = proxy.out_link().target(dest_gid); 
        proxy.enqueue(dest, volume_block->m_partials[i]);
      } //for

      volume_block->m_partials.clear();

    } // if
    else
    {
      size_t total_size = 0;
      //
      // count the total incoming partials
      //
      for(int i = 0; i < proxy.in_link().size(); ++i)
      {
        int gid = proxy.in_link().target(i).gid;
        assert(gid == i);
        diy::MemoryBuffer &incoming = proxy.incoming(gid);
        size_t incoming_size = incoming.size() / sizeof(AbsorptionPartial);
        total_size += incoming_size; 
      } // for
      
      volume_block->m_partials.resize(total_size);
      ROVER_INFO("Recieving "<<total_size<<" partials");

      size_t current_offset = 0;
      for(int i = 0; i < proxy.in_link().size(); ++i)
      {
        int gid = proxy.in_link().target(i).gid;
        assert(gid == i);
        diy::MemoryBuffer &incoming = proxy.incoming(gid);
        size_t incoming_size = incoming.size() / sizeof(AbsorptionPartial);
        std::copy( (AbsorptionPartial*) &incoming.buffer[0],
                   (AbsorptionPartial*) &incoming.buffer[0] + incoming_size,
                   &volume_block->m_partials[current_offset] );
        current_offset += incoming_size;
      } // for


    } // else

  } // operator
};

//
// Collect struct sends all data to a single node.
//
struct Collect 
{
  const diy::RegularDecomposer<diy::ContinuousBounds> &m_decomposer;

  Collect(const diy::RegularDecomposer<diy::ContinuousBounds> &decomposer)
    : m_decomposer(decomposer)
  {}

  void operator()(void *v_block, const diy::ReduceProxy &proxy) const
  {
    AbsorptionBlock *volume_block = static_cast<AbsorptionBlock*>(v_block);
    //
    // first round we have no incoming. Take the partials we have
    // and sent them to to the right rank
    //
    if(proxy.in_link().size() == 0)
    {
      const int size = volume_block->m_partials.size(); 
      ROVER_INFO("Processing partials block of size "<<size);

      for(int i = 0; i < size; ++i)
      {
        int dest_gid =  0;
        diy::BlockID dest = proxy.out_link().target(dest_gid); 
        proxy.enqueue(dest, volume_block->m_partials[i]);
      } //for

      volume_block->m_partials.clear();

    } // if
    else
    {
      size_t total_size = 0;
      //
      // count the total incoming partials
      //
      for(int i = 0; i < proxy.in_link().size(); ++i)
      {
        int gid = proxy.in_link().target(i).gid;
        assert(gid == i);
        diy::MemoryBuffer &incoming = proxy.incoming(gid);
        size_t incoming_size = incoming.size() / sizeof(AbsorptionPartial);
        total_size += incoming_size; 
      } // for
      
      volume_block->m_partials.resize(total_size);
      ROVER_INFO("Collecting "<<total_size<<" partials");

      size_t current_offset = 0;
      for(int i = 0; i < proxy.in_link().size(); ++i)
      {
        int gid = proxy.in_link().target(i).gid;
        assert(gid == i);
        diy::MemoryBuffer &incoming = proxy.incoming(gid);
        size_t incoming_size = incoming.size() / sizeof(AbsorptionPartial);
        std::copy( (AbsorptionPartial*) &incoming.buffer[0],
                   (AbsorptionPartial*) &incoming.buffer[0] + incoming_size,
                   &volume_block->m_partials[current_offset] );
        current_offset += incoming_size;
      } // for


    } // else

  } // operator
};
} // namespace anon
void redistribute(std::vector<AbsorptionPartial> &partials, 
                  MPI_Comm comm,
                  const int &domain_min_pixel,
                  const int &domain_max_pixel)
{
  diy::mpi::communicator world(comm);
  diy::DiscreteBounds global_bounds;
  global_bounds.min[0] = domain_min_pixel;
  global_bounds.max[0] = domain_max_pixel;
  
  // tells diy to use all availible threads
  const int num_threads = -1; 
  const int num_blocks = world.size(); 
  const int magic_k = 2;

  diy::Master master(world, num_threads);
  
  // create an assigner with one block per rank
  diy::ContiguousAssigner assigner(num_blocks, num_blocks); 
  AddAbsorptionBlock create(master, partials);

  const int dims = 1;
  diy::RegularDecomposer<diy::DiscreteBounds> decomposer(dims, global_bounds, num_blocks);
  decomposer.decompose(world.rank(), assigner, create);
  
  diy::all_to_all(master, assigner, Redistribute(decomposer), magic_k);
}

//
// collect uses the all-to-all construct to perform a gather to
// the root rank. All other ranks will have no data
//
void collect(std::vector<AbsorptionPartial> &partials,
             MPI_Comm comm)
{
  diy::mpi::communicator world(comm);
  diy::ContinuousBounds global_bounds;
  global_bounds.min[0] = 0;
  global_bounds.max[0] = 1;
  
  // tells diy to use all availible threads
  const int num_threads = -1; 
  const int num_blocks = world.size(); 
  const int magic_k = 2;

  diy::Master master(world, num_threads);
  
  // create an assigner with one block per rank
  diy::ContiguousAssigner assigner(num_blocks, num_blocks); 
  AddAbsorptionBlock create(master, partials);

  const int dims = 1;
  diy::RegularDecomposer<diy::ContinuousBounds> decomposer(dims, global_bounds, num_blocks);
  decomposer.decompose(world.rank(), assigner, create);
  
  diy::all_to_all(master, assigner, Collect(decomposer), magic_k);

  ROVER_INFO("Ending size: "<<partials.size()<<"\n");
   
}

} //namespace rover


#endif
