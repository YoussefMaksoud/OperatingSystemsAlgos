// ======================================================================
// You must modify this file and then submit it for grading to D2L.
// ======================================================================
//
// count_pi() calculates the number of pixels that fall into a circle
// using the algorithm explained here:
//
// https://en.wikipedia.org/wiki/Approximations_of_%CF%80
//
// count_pi() takes 2 paramters:
//  r         =  the radius of the circle
//  n_threads =  the number of threads you should create
//
// Currently the function ignores the n_threads parameter. Your job is to
// parallelize the function so that it uses n_threads threads to do
// the computation.

#include "calcpi.h"
#include <pthread.h>
#include <iostream>

using namespace std;

struct Task{
  int start_row;
  int end_row;
  int r;
  uint64_t partial_count;
}tasks[256];


void* thread_pi(void* task_index){
  int index = (long int) task_index;
  double rsq = double(tasks[index].r) * tasks[index].r;

  for( double x = -tasks[index].r ; x <= tasks[index].r; x ++)
    for( double y = tasks[index].start_row ; y <= tasks[index].end_row; y ++)
      if( x*x + y*y <= rsq) tasks[index].partial_count ++; // if the point falls within the circle 
  
  return (void*) tasks[index].partial_count;

}

uint64_t count_pi(int r, int n_threads)
{

  pthread_t thread_arr[n_threads];
  uint64_t count = 0;

  for(int i = 0; i < n_threads; i++){
    tasks[i].start_row = -r + (2*r+1) * i/n_threads;
    tasks[i].end_row = -r + (2*r+1) * (i+1)/n_threads;
    tasks[i].r = r;
  }

  for(int i = 0; i < n_threads; i++){
    pthread_create(&thread_arr[i], NULL, thread_pi, (void*) i);
  }

  for(int i = 0; i < n_threads; i++){
    pthread_join(thread_arr[i], 0);
  }


  for(int i = 0; i < n_threads; i++){
    count += tasks[i].partial_count;
  }

  return count;
}
