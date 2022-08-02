// this is the file you need to edit

#include "fatsim.h"
#include <cstdio>
#include <unordered_map>
#include <iostream>

using namespace std;

struct Node{
  long current_val;
  long next_val;
  int is_visited;
};

void confirm(vector<long> &src, unordered_map<long, bool> &trgt);
// reimplement this function
void fat_sim(const std::vector<long> & fat, long & longest_file_blocks, long & unused_blocks)
{
  int inDegree[fat.size()]; // vector keeping track of how many things point to an index

  for(int i = 0; i < fat.size(); i++){
    inDegree[i] = 0;
  }

  vector<int64_t> leafNodes; // vector that contains the indices of all leafnodes
  unordered_map<long, bool> confirmed_files;
  vector<long> potential_files;
  unordered_map<long, Node> graph;

  longest_file_blocks = 0;
  unused_blocks = 0;
  

  for(int i = 0; i < fat.size(); i++){
    
      Node newNode;
      newNode.current_val = i;
      newNode.next_val = fat[i];
      newNode.is_visited = 0;
   
      graph.emplace(i, newNode); // insert the current node index and the node in the unordered map 

      if(graph[i].next_val != -1){
         inDegree[graph[i].next_val] ++;// increment the next node since the current is pointing to it
      }

  }

  for(int i = 0; i < fat.size(); i++){ // populate the leafNode vector to start our DFS
    if(inDegree[i] == 0 || inDegree[i] == -1){
      leafNodes.push_back(i);
      
    }
  }

//------------------------------------------------------------------------------------------------------------------------------------------//
  //start the depth first search
  Node *current_node;
   
  for(uint64_t leaf_root = 0; leaf_root < leafNodes.size(); leaf_root++){
     current_node = &graph[leafNodes[leaf_root]];
     long temp_longest = 1;

     cout << "\n\n\n";
     while(current_node->next_val != -1){
       cout << current_node->current_val << "\n";
       if(graph[current_node->current_val].is_visited == 1 && confirmed_files.find(current_node->current_val) == confirmed_files.end()){             
         potential_files.clear();
         leaf_root++;
         break;
       }
       
       current_node->is_visited = 1;
       temp_longest++;
       potential_files.push_back(current_node->current_val);
       current_node = &graph[current_node->next_val];
     }
     if(current_node->next_val == -1){
      potential_files.push_back(current_node->current_val);
      if(temp_longest > longest_file_blocks){
        longest_file_blocks = temp_longest;
      }
      confirm(potential_files, confirmed_files);
      potential_files.clear();
     }
  }
  
  unused_blocks = fat.size() - confirmed_files.size();

  return;

}


void confirm(vector<long> &src, unordered_map<long, bool> &trgt){
  for(uint64_t i = 0; i < src.size(); i ++){
      trgt.emplace(src[i], true);   
  }
}
