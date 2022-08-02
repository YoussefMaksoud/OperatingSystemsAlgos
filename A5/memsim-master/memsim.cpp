// this is the file you need to edit

#include "memsim.h"
#include <list>
#include <set>
#include <unordered_map>
#include <stdlib.h> 
#include <iostream>
#include<cassert>
//#include <vector>

using namespace std;



// feel free to use this class as a starting point

struct Partition {
  long tag; // if there is a number then it is allocated to that tag, if it is 0 then it is free
  long size;
  int64_t addr;

  Partition(long t, long s, int64_t a){
    tag = t;
    size = s;
    addr = a;
  }

};

typedef list<Partition>::iterator PartitionRef;

struct scmp {
  bool operator()(const PartitionRef & c1, const PartitionRef & c2) const
  {
    if (c1->size == c2->size) {
      return c1->addr < c2->addr;
    } else {
      return c1->size < c2->size;
    }
  }
};

struct Simulator {
  Simulator(int64_t page_size) { page_size_ = page_size; }
  void allocate(int tag, int size); 
  void deallocate(int tag); 
  void getStats(MemSimResult & result);

  int64_t times_requested = 0;

  int64_t page_size_; // size of the page

  list<Partition> partitions; // all partitions to be able to access prev(), next() nodes

  set<PartitionRef, scmp> free_blocks; // sorted partitions by size/ address

  unordered_map<long, vector<PartitionRef>>
      tagged_blocks; // quick access to all tagged partitions tag, all partiions list
};

scmp comp;

void Simulator::allocate(int tag, int size){
  
  //only need to check if we must request a new page(s) when allocating

  if(times_requested == 0){ // if the parttion list is empty
      int request_num;
       request_num = size/page_size_;

       if((request_num*page_size_) < size){
         request_num++;
       }
      times_requested += request_num; // increase the number of page requests
      Partition new_request = {0, (request_num * page_size_), 0}; // create a parttion with size request_num*page_size_
      PartitionRef new_part = partitions.insert(partitions.begin(), new_request); // place the new free partition in the partitions list
      free_blocks.emplace(new_part); // add it to the free_blocks set 
      

  }
//----------------------------------------------------------------------------------------------------------------------------------------------------------//


  list<Partition> dummy {Partition(-1, size, 0)}; // create a dummy linked list to extract an iterator from it
  auto sbesti = free_blocks.lower_bound(dummy.begin()); // set the iterator to the best fit free partition for the request // points to the partitons list
  PartitionRef best_free_block_iter = partitions.end(); // best fit iterator to be set

  if(sbesti != free_blocks.end()){ // if the "best fit" partition actually exists in the free_blocks set
    best_free_block_iter = *sbesti; // point it to that partition in the partitions list
  }
  else if(best_free_block_iter == partitions.end()){ // then we need to allocate more memory

     if(prev(partitions.end())->tag == 0){ // if the last partiton is free

      PartitionRef to_add = prev(partitions.end());
      free_blocks.erase(to_add);
      
       int request_num = size/page_size_;
       if((prev(partitions.end())->size + (request_num*page_size_)) < size){
         request_num++;
       }

       times_requested += request_num; // increase the total page requests

       to_add->size += (request_num * page_size_); // extend and merge the partition

        free_blocks.emplace(to_add);

       best_free_block_iter = prev(partitions.end()); // point the best fit itreator to the last element

     }
     else if(prev(partitions.end())->tag > 0){ // if the last partition is occupied
       int request_num;

       request_num = size/page_size_;

       if((request_num*page_size_) < size){
         request_num++;
       }

       times_requested += request_num;
       Partition new_request(0, (request_num * page_size_), prev(partitions.end())->addr + prev(partitions.end())->size); // create a partition with size request_num*page_size_ and address is the end of the previous block
       PartitionRef new_part = partitions.insert(partitions.end(), new_request); // place the new free partition in the partitions list
       free_blocks.emplace(new_part); // add it to the free_blocks set

       best_free_block_iter = prev(partitions.end()); // point the best fit itreator to the last element

     }

     
  }
  //now we use best free block iterator to allocate memory for the request 

  if(size == best_free_block_iter->size){

    free_blocks.erase(best_free_block_iter);
    best_free_block_iter->tag = tag;  
    tagged_blocks[tag].push_back(best_free_block_iter);

  }
  else if(size < best_free_block_iter->size){
    free_blocks.erase(best_free_block_iter);
    Partition new_occ(tag, size, best_free_block_iter->addr); // make new proper sized free partition
    best_free_block_iter->addr += size; // set new address of the free block
    best_free_block_iter->size -= size; // adjust size of free block

    free_blocks.emplace(best_free_block_iter);
    partitions.insert(best_free_block_iter, new_occ); //insert the occupied partition into partitions vector (before free chunk)
    tagged_blocks[tag].push_back(prev(best_free_block_iter)); // add the new occupied chunk to the tagged blocks
  }


}


void Simulator::deallocate(int tag){

  if(!partitions.empty() && (tagged_blocks.find(tag) != tagged_blocks.end())){
  // //look for tag in tagged_blocks to avoid looping through the whole partition list
  PartitionRef after_part; // checks partion after the freed one to see if it is also free
  PartitionRef before_part; // checks partion before the freed one to see if it is also free
  
  for(uint64_t i = 0; i < tagged_blocks[tag].size(); i++){
    tagged_blocks[tag].at(i)->tag = 0;
    
    after_part = next(tagged_blocks[tag].at(i));
    before_part = prev(tagged_blocks[tag].at(i));


     // point it to next partition //------check to make sure not out of scope------//
    if(after_part != partitions.end()){
      if(after_part->tag == 0){ // if the next block is free
        free_blocks.erase(after_part); //erase parttion from free_blocks
        tagged_blocks[tag].at(i)->size += after_part->size; // increase free partiton size
        partitions.erase(after_part); //erase the after partition __________________________________
        
      }
    }


    // point to previous partition
    if(before_part != prev(partitions.begin())){
      if(before_part->tag == 0){ // if the previoous block is free
        free_blocks.erase(before_part); // erase the free parttion from free_blocks
        tagged_blocks[tag].at(i)->size += before_part->size; // increase free partition size
        tagged_blocks[tag].at(i)->addr = before_part->addr; // change the address of the parttion size
        partitions.erase(before_part); // erase the before partiton

      }
    }

    free_blocks.emplace(tagged_blocks[tag].at(i)); // add the extra big partition to the free_blocks
  }

  //   //tagged_blocks[tag].clear();
    tagged_blocks.erase(tag); // remove the tag from tagged_blocks
    
  }
  
}

void Simulator:: getStats(MemSimResult & result){
    //return the size of the maximum free partition (set to 0 if no free partitions exist)
    //return the total number of pages requested

    if(free_blocks.size() == 0){
      result.max_free_partition_size = 0; // if free_blocks set is empty, then there is no free partitions
    }
    else{
      result.max_free_partition_size = (*(prev(free_blocks.end())))->size; // point it to top of bablanced tree, since it is ordered, top is biggest
    }
    
    result.n_pages_requested = times_requested; // set the requested pages to the munber of times a page was requested
}

void mem_sim(int64_t page_size, const std::vector<Request> & requests, MemSimResult & result)
{

  Simulator sim(page_size);
  //check the tag in a for loop for all requests:

  for(uint64_t i = 0; i < requests.size(); i++){
    
    if(requests[i].tag > 0){ //if the tag is positive were gonna allocate memory
      sim.allocate(requests.at(i).tag, requests.at(i).size); // pass the tag and size of request to be allocated
    }if(requests[i].tag < 0){ // else if the tag is negative we are going to deallocate the absolute value of that tag
      int abs_tag = abs(requests[i].tag); // save the absolute value of tag 
      sim.deallocate(abs_tag); // pass the absolute value of the tag into the deallocate function
    }
  }

  sim.getStats(result);

  
  return;
}

