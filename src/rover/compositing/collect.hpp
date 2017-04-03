#ifndef rover_compositing_collect_h
#define rover_compositing_collect_h

#include <compositing/absorption_partial.hpp>
#include <compositing/volume_partial.hpp>
#include <diy/assigner.hpp>
#include <diy/decomposition.hpp>
#include <diy/master.hpp>
#include <diy/reduce-operations.hpp>
#include <utils/rover_logging.hpp>

namespace rover {
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
    const int collection_rank = 0; 
    if(proxy.in_link().size() == 0 && proxy.gid() != collection_rank)
    {
      ROVER_INFO("Collect sending partials vector "<<block->m_partials.size());
      int dest_gid = collection_rank;
      diy::BlockID dest = proxy.out_link().target(dest_gid); 
      proxy.enqueue(dest, block->m_partials);

      block->m_partials.clear();

    } // if
    else if(proxy.gid() == collection_rank)
    {
      
      for(int i = 0; i < proxy.in_link().size(); ++i)
      {
        int gid = proxy.in_link().target(i).gid;
        if(gid == collection_rank)
        {
          continue;
        }
        //TODO: leave the paritals that start here, here
        ROVER_INFO("dequeuing from "<<gid);
        std::vector<typename BlockType::PartialType> incoming_partials;
        proxy.dequeue(gid, incoming_partials); 
        const int incoming_size = incoming_partials.size();
        ROVER_INFO("dequeuing "<<incoming_size);
        // TODO: make this a std::copy
        for(int j = 0; j < incoming_size; ++j)
        {
          block->m_partials.push_back(incoming_partials[j]);
        }
      } // for
    } // else

  } // operator
};

//
// collect uses the all-to-all construct to perform a gather to
// the root rank. All other ranks will have no data
//
template<typename AddBlockType>
void collect_detail(std::vector<typename AddBlockType::PartialType> &partials,
                    MPI_Comm comm)
{
  typedef typename AddBlockType::Block Block;

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
  AddBlockType create(master, partials);

  const int dims = 1;
  diy::RegularDecomposer<diy::ContinuousBounds> decomposer(dims, global_bounds, num_blocks);
  decomposer.decompose(world.rank(), assigner, create);
  
  diy::all_to_all(master, assigner, Collect<Block>(decomposer), magic_k);

  ROVER_INFO("Ending size: "<<partials.size()<<"\n");
   
}

template<typename T> 
void collect(std::vector<T> &partials,
             MPI_Comm comm);

template<> 
void collect<VolumePartial>(std::vector<VolumePartial> &partials,
                            MPI_Comm comm)
{
  collect_detail<AddBlock<VolumeBlock>>(partials, comm);
}

template<> 
void collect<AbsorptionPartial<double>>(std::vector<AbsorptionPartial<double>> &partials,
                                        MPI_Comm comm)
{
  collect_detail<AddBlock<AbsorptionBlock<double>>>(partials, comm);
}

template<> 
void collect<AbsorptionPartial<float>>(std::vector<AbsorptionPartial<float>> &partials,
                                       MPI_Comm comm)
{
  collect_detail<AddBlock<AbsorptionBlock<float>>>(partials, comm);
}

} // namespace rover

#endif
