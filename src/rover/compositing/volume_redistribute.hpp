#ifndef rover_compositing_redistribute_h
#define rover_compositing_redistribute_h

#include <diy/assigner.hpp>
#include <diy/decomposition.hpp>
#include <diy/master.hpp>
#include <diy/reduce-operations.hpp>
#include <compositing/volume_block.hpp>

namespace rover{

struct Redistribute
{
  const diy::RegularDecomposer<diy::DiscreteBounds> &m_decomposer;

  Redistribute(const diy::RegularDecomposer<diy::DiscreteBounds> &decomposer)
    : m_decomposer(decomposer)
  {}

  void operator()(void *v_block, const diy::ReduceProxy &proxy) const
  {
    VolumeBlock *volume_block = static_cast<VolumeBlock*>(v_block);
    //
    // first round we have no incoming. Take the partials we have
    // and sent them to to the right rank
    //
    if(proxy.in_link().size() == 0)
    {
      const int size = volume_block->m_partials.size(); 
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
        size_t incoming_size = incoming.size() / sizeof(VolumePartial);
        total_size += incoming_size; 
      } // for
      
      volume_block->m_partials.resize(total_size);
  
      size_t current_offset = 0;
      for(int i = 0; i < proxy.in_link().size(); ++i)
      {
        int gid = proxy.in_link().target(i).gid;
        assert(gid == i);
        diy::MemoryBuffer &incoming = proxy.incoming(gid);
        size_t incoming_size = incoming.size() / sizeof(VolumePartial);
        std::copy( (VolumePartial*) &incoming.buffer[0],
                   (VolumePartial*) &incoming.buffer[0] + incoming_size,
                   &volume_block->m_partials[current_offset] );
        current_offset += incoming_size;
      } // for


    } // else

  } // operator
};

void volume_redistribute(std::vector<VolumePartial> &partials, 
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
  AddVolumeBlock create(master, partials);

  const int dims = 1;
  diy::RegularDecomposer<diy::DiscreteBounds> decomposer(dims, global_bounds, num_blocks);
  decomposer.decompose(world.rank(), assigner, create);
  
  diy::all_to_all(master, assigner, Redistribute(decomposer), magic_k);
}

} //namespace rover


#endif
