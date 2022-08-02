// ======================================================================
// You must modify this file and then submit it for grading to D2L.
// ======================================================================

#include "sumFactors.h"
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <pthread.h>
#include <vector>
#include <atomic>
#include <limits>

using namespace std;

struct Task
{
  int64_t start_num;
  int64_t end_num;
  int64_t num;
};

//global sum
int64_t sum;
//subprogram finished flag
bool finished;
vector<Task> tasks;
//declare a barrier object as global
pthread_barrier_t barrier;
//number of threads used
int num_threads;

//atomic varibale to keep track of the smallest divisor for a gien number read
atomic<int64_t> smallest_divisor_found(numeric_limits<int64_t>::max());

//atomic comparison function
template <typename T>
void update_min(atomic<T> &av, T const &v) noexcept
{
  T prev = av;
  while (prev > v && !av.compare_exchange_weak(prev, v))
    ;
}

//reads input from command line
void readInput()
{
  //number being read
  int64_t num;

  //if no more numbers read, raise flag
  if (!(std::cin >> num))
  {
    finished = true;
  }
  else
  {
    //divide up the work evenly between threads
    int64_t sqrtnum = sqrt(num);

    for (int tid = 0; tid < num_threads; tid++)
    {
      tasks[tid].start_num = ((tid * (sqrtnum + 1)) / num_threads);
      tasks[tid].end_num = ((tid + 1) * (sqrtnum + 1)) / num_threads;
      tasks[tid].num = num;
      //cout << tasks[tid].num << " " << tasks[tid].start_num << " " << tasks[tid].end_num << endl;
    }
  }
}

// calculates smallest divisor of n that is less than n
//
// returns 0 if n is prime or n < 2
//
// examples:
//    n =  1         --> result = 0
//    n =  2         --> result = 0
//    n =  9         --> result = 3
//    n = 35         --> result = 5
//    n = 8589934591 --> result = 7
//
// you may modify this function
//

//gets the smallest divisor for a number
void *get_smallest_divisor(void *t_index)
{

  while (1) //repeat forever
  {
    int sel = pthread_barrier_wait(&barrier);
    if (sel == PTHREAD_BARRIER_SERIAL_THREAD)
    {
      //serial task 1 read the input
      readInput();
    }

    pthread_barrier_wait(&barrier);
    // if finished exit thread
    if (finished == true)
    {
      break;
    }

    int index = (long int)t_index;
    if (tasks[index].num <= 3)
    {
      update_min(smallest_divisor_found, (int64_t)0); // 2 and 3 are primes
      //finished = true;
      //cout <<"in if 1";
    }
    else if (tasks[index].num % 2 == 0)
    {
      update_min(smallest_divisor_found, (int64_t)2); // handle multiples of 2
      //finished = true;
      //cout << "in if 2";
    }
    else if (tasks[index].num % 3 == 0)
    {
      update_min(smallest_divisor_found, (int64_t)3); // handle multiples of 3
      //finished = true;
      //cout << "in if 3";
    }

    //cout << tasks[index].num << " " << tasks[index].start_num << " " << tasks[index].end_num << endl;
    int64_t i;
    for (i = 5; i < tasks[index].start_num; i += 6)
      ;

    if (i != 0)
    {
      while (i <= tasks[index].end_num)
      {
        if (tasks[index].num % i == 0)
        {
          update_min(smallest_divisor_found, (int64_t)i);
          //sum += smallest_divisor_found;
          break;
        }
        else if (tasks[index].num % (i + 2) == 0)
        {
          update_min(smallest_divisor_found, i + 2);
          //sum += smallest_divisor_found;
          break;
        }
        i += 6;
      }

      //cout << smallest_divisor_found << endl;
    }

    if (smallest_divisor_found == numeric_limits<int64_t>::max() && (long int)t_index == num_threads - 1)
    {
      smallest_divisor_found = 0;
    }

    sel = pthread_barrier_wait(&barrier);
    if (sel == PTHREAD_BARRIER_SERIAL_THREAD)
    {
      //cout << tasks[index].num << " " << tasks[index].start_num << " " << tasks[index].end_num << endl; //<< "small  " << smallest_divisor_found << endl;
      sum += smallest_divisor_found;
      smallest_divisor_found = numeric_limits<int64_t>::max();
    }
  }
  //cout << tasks[index].partial_sum << endl;

  pthread_exit(0);
}

// reads in a positive integers in range (2 .. 2^63-1) from standard input
// for each number on input:
//   if the number is prime, it is ignored
//   determines the smallest factor of the composite number
// prints out the sum of all smallest factors
//
// if no composite numbers are found, it returns 0
//
// please note: the current implementation is single-threades, so
// n_threads argument is ignored
//
// you may modify this function
//

int64_t sum_factors(int n_threads)
{

  num_threads = n_threads;
  int bi = pthread_barrier_init(&barrier, NULL, n_threads);
  if (bi != 0)
  {
    cout << "unable to initialize barrier";
  }

  //declare thread array
  pthread_t thread_arr[n_threads];

  sum = 0;
  finished = false;

  //create memory area for threads
  for (int i = 0; i < n_threads; i++)
  {
    Task newTask = {0, 0, 0};
    tasks.push_back(newTask);
  }
  //start the threads
  for (int64_t i = 0; i < n_threads; i++)
  {
    int rc = pthread_create(&thread_arr[i], NULL, get_smallest_divisor, (void *)i);
    if (rc != 0)
    {
      cout << "unable to create array";
    }
  }
  //join the threads
  for (int i = 0; i < n_threads; i++)
  {
    pthread_join(thread_arr[i], 0);
  }
  //destroy the barrier
  pthread_barrier_destroy(&barrier);

  //return the total sum of smallest divisors
  return sum;
}
