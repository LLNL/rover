#ifndef rover_compositing_redistribute_h
#define rover_compositing_redistribute_h

#include <diy/assigner.hpp>
#include <diy/decomposition.hpp>
#include <diy/master.hpp>
#include <diy/reduce-operations.hpp>
#include <compositing/volume_block.hpp>
#include <utils/rover_logging.hpp>
namespace rover{
//
// Redistributes partial composites to the ranks that owns 
// that sectoon of the image. Currently, the domain is decomposed
// in 1-D from min_pixel to max_pixel.
//
template<typename BlockType>
struct Redistribute
{
  const diy::RegularDecomposer<diy::DiscreteBounds> &m_decomposer;

  Redistribute(const diy::RegularDecomposer<diy::DiscreteBounds> &decomposer)
    : m_decomposer(decomposer)
  {}

  void operator()(void *v_block, const diy::ReduceProxy &proxy) const
  {
    BlockType *block = static_cast<BlockType*>(v_block);
    //
    // first round we have no incoming. Take the partials we have
    // and sent them to to the right rank
    //
    if(proxy.in_link().size() == 0)
    {
      const int size = block->m_partials.size(); 
      ROVER_INFO("Processing partials block of size "<<size);

      for(int i = 0; i < size; ++i)
      {
        diy::Point<int,DIY_MAX_DIM> point;
        point[0] = block->m_partials[i].m_pixel_id;
        int dest_gid = m_decomposer.point_to_gid(point);
        diy::BlockID dest = proxy.out_link().target(dest_gid); 
        proxy.enqueue(dest, block->m_partials[i]);
      } //for

      block->m_partials.clear();

    } // if
    else
    {
      for(int i = 0; i < proxy.in_link().size(); ++i)
      {
        int gid = proxy.in_link().target(i).gid;
        
        if(gid == proxy.gid())
        {
          continue;
        }
        std::vector<typename BlockType::PartialType> incoming_partials;
        proxy.dequeue(gid, incoming_partials); 
        const int incoming_size = incoming_partials.size();
        // TODO: make this a std::copy
        for(int j = 0; j < incoming_size; ++i)
        {
          block->m_partials.push_back(incoming_partials[j]);
        }
        /* Fast path for plain old data
        diy::MemoryBuffer &incoming = proxy.incoming(gid);
        size_t incoming_size = incoming.size() / sizeof(VolumePartial);
        total_size += incoming_size; 
        */
      } // for

    } // else

  } // operator
};

//
// Collect struct sends all data to a single node.
//
template<typename BlockType>
struct Collect 
{
  const diy::RegularDecomposer<diy::ContinuousBounds> &m_decomposer;

  Collect(const diy::RegularDecomposer<diy::ContinuousBounds> &decomposer)
    : m_decomposer(decomposer)
  {}

  void operator()(void *v_block, const diy::ReduceProxy &proxy) const
  {
    BlockType *block = static_cast<BlockType*>(v_block);
    //
    // first round we have no incoming. Take the partials we have
    // and sent them to to the right rank
    //
    if(proxy.in_link().size() == 0)
    {
      const int size = block->m_partials.size(); 
      ROVER_INFO("Processing partials block of size "<<size);

      for(int i = 0; i < size; ++i)
      {
        int dest_gid =  0;
        diy::BlockID dest = proxy.out_link().target(dest_gid); 
        proxy.enqueue(dest, block->m_partials[i]);
      } //for

      block->m_partials.clear();

    } // if
    else
    {
      
      for(int i = 0; i < proxy.in_link().size(); ++i)
      {
        int gid = proxy.in_link().target(i).gid;
        
        if(gid == proxy.gid())
        {
          continue;
        }
        std::vector<typename BlockType::PartialType> incoming_partials;
        proxy.dequeue(gid, incoming_partials); 
        const int incoming_size = incoming_partials.size();
        // TODO: make this a std::copy
        for(int j = 0; j < incoming_size; ++i)
        {
          block->m_partials.push_back(incoming_partials[j]);
        }
      } // for
    } // else

  } // operator
};

template<typename AddBlockType>
void redistribute_detail(std::vector<typename AddBlockType::PartialType> &partials, 
                         MPI_Comm comm,
                         const int &domain_min_pixel,
                         const int &domain_max_pixel)
{
  typedef typename AddBlockType::Block Block;

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
  AddBlockType create(master, partials);

  const int dims = 1;
  diy::RegularDecomposer<diy::DiscreteBounds> decomposer(dims, global_bounds, num_blocks);
  decomposer.decompose(world.rank(), assigner, create);
  
  diy::all_to_all(master, assigner, Redistribute<Block>(decomposer), magic_k);
}


template<typename T>
void redistribute(std::vector<T> &partials, 
                  MPI_Comm comm,
                  const int &domain_min_pixel,
                  const int &domain_max_pixel)
{
  ROVER_ERROR("Redistribute default template. SHOULD NOT HAPPEN");
}

template<>
void redistribute<VolumePartial>(std::vector<VolumePartial> &partials, 
                                                            MPI_Comm comm,
                                                            const int &domain_min_pixel,
                                                            const int &domain_max_pixel)
{
  redistribute_detail<AddBlock<VolumeBlock> >(partials,
                                              comm,
                                              domain_min_pixel,
                                              domain_max_pixel);
}

  

//
// collect uses the all-to-all construct to perform a gather to
// the root rank. All other ranks will have no data
//
void collect(std::vector<VolumePartial> &partials,
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
  AddBlock<VolumeBlock> create(master, partials);

  const int dims = 1;
  diy::RegularDecomposer<diy::ContinuousBounds> decomposer(dims, global_bounds, num_blocks);
  decomposer.decompose(world.rank(), assigner, create);
  
  diy::all_to_all(master, assigner, Collect<VolumeBlock>(decomposer), magic_k);

  ROVER_INFO("Ending size: "<<partials.size()<<"\n");
   
}

} //namespace rover


#endif
