#ifndef rover_compositing_redistribute_h
#define rover_compositing_redistribute_h

#include <compositing/volume_partial.hpp>
#include <compositing/blocks.hpp>
#include <diy/assigner.hpp>
#include <diy/decomposition.hpp>
#include <diy/master.hpp>
#include <diy/reduce-operations.hpp>
#include <map>
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
      std::map<diy::BlockID,std::vector<typename BlockType::PartialType>> outgoing;
       
      for(int i = 0; i < size; ++i)
      {
        diy::Point<int,DIY_MAX_DIM> point;
        point[0] = block->m_partials[i].m_pixel_id;
        int dest_gid = m_decomposer.point_to_gid(point);
        diy::BlockID dest = proxy.out_link().target(dest_gid); 
        outgoing[dest].push_back(block->m_partials[i]);
      } //for
       
      block->m_partials.clear();
      
      ROVER_INFO("out setup ");
      
      for(int i = 0; i < proxy.out_link().size(); ++i)
      {
        int dest_gid = proxy.out_link().target(i).gid;
        diy::BlockID dest = proxy.out_link().target(dest_gid); 
        proxy.enqueue(dest, outgoing[dest]);
        //outgoing[dest].clear();
      }

    } // if
    else
    {
      ROVER_INFO("getting "<<proxy.in_link().size()<<" blocks"); 
      size_t total = 0;
#if 1 
      for(int i = 0; i < proxy.in_link().size(); ++i)
      {
        int gid = proxy.in_link().target(i).gid;
        std::vector<typename BlockType::PartialType> incoming_partials;
        ROVER_INFO("dequing from "<<gid);
        proxy.dequeue(gid, incoming_partials); 
        const int incoming_size = incoming_partials.size();
        ROVER_INFO("Incoming size "<<incoming_size<<" from "<<gid);
        // TODO: make this a std::copy
        for(int j = 0; j < incoming_size; ++j)
        {
          block->m_partials.push_back(incoming_partials[j]);
        }
      } // for
#else
      for(int i = 0; i < proxy.in_link().size(); ++i)
      {
        int gid = proxy.in_link().target(i).gid;
        /*
        if(gid == proxy.gid())
        {
          continue;
        }*/
        diy::MemoryBuffer &incoming = proxy.incoming(gid);
        size_t incoming_sz = incoming.size() / sizeof(VolumePartial);
        ROVER_INFO("Incoming size "<<incoming_sz);
        total += incoming_sz;
  
      } // for
      block->m_partials.resize(total);
      size_t sz = 0;
      for(int i = 0; i < proxy.in_link().size(); ++i)
      {
        int gid = proxy.in_link().target(i).gid;
        diy::MemoryBuffer &incoming = proxy.incoming(gid);
        size_t incoming_sz = incoming.size() / sizeof(VolumePartial);
        std::copy((VolumePartial*) &incoming.buffer[0],
                  (VolumePartial*) &incoming.buffer[0] + incoming_sz,
                  &block->m_partials[sz]);
        sz += incoming_sz;
      }
#endif

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

//
// Define a default template that cannot be instantiated
//
template<typename T>
void redistribute(std::vector<T> &partials, 
                  MPI_Comm comm,
                  const int &domain_min_pixel,
                  const int &domain_max_pixel);

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

template<>
void redistribute<AbsorptionPartial<double>>(std::vector<AbsorptionPartial<double>> &partials, 
                                             MPI_Comm comm,
                                             const int &domain_min_pixel,
                                             const int &domain_max_pixel)
{
  redistribute_detail<AddBlock<AbsorptionBlock<double>>>(partials,
                                                         comm,
                                                         domain_min_pixel,
                                                         domain_max_pixel);
}

template<>
void redistribute<AbsorptionPartial<float>>(std::vector<AbsorptionPartial<float>> &partials, 
                                            MPI_Comm comm,
                                            const int &domain_min_pixel,
                                            const int &domain_max_pixel)
{
  redistribute_detail<AddBlock<AbsorptionBlock<float>>>(partials,
                                                        comm,
                                                        domain_min_pixel,
                                                        domain_max_pixel);
}

} //namespace rover

#endif
