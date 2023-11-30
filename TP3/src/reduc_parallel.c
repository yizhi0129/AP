//
#define _GNU_SOURCE

//
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

//
#include "types.h"

//
typedef struct thread_data_s {

  //Thread ID
  //POSIX.1 norm system unix: doc man
  pthread_t id; 
  
  //Number of elements handled by a thread
  u64 n;

  //Array of elements
  //pointer, independant each thread
  f64 *restrict a;

  //Partial sum
  f64 r;
  
} thread_data_t;

//3 methods to initialize the array
void init(f64 *restrict a, u64 n, u8 type)
{
  //Initialize with random values
  if (type == 'r')
    for (u64 i = 0; i < n; i++)
      a[i] = (f64)RAND_MAX / (f64)rand();
  else //Clear
    if (type == 'z')
      for (u64 i = 0; i < n; i++)
	a[i] = 0.0;
    else //Initialize with a random constant
      if (type == 'c')
	{
	  const f64 c = (f64)RAND_MAX / (f64)rand();
	  
	  for (u64 i = 0; i < n; i++)
	    a[i] = c;
	}
}

//sum
f64 reduc_sequential(f64 *restrict a, u64 n)
{
  f64 r = 0.0;
  
  for (u64 i = 0; i < n; i++)
    r += a[i];
  
  return r;
}

//thread: a function here
void *_reduc_(void *p)
{
  thread_data_t *td = (thread_data_t *)p; //td : thread data
  
  td->r = reduc_sequential(td->a, td->n);
  
  return NULL; // void * : should return a pointer, NULL here
}

//thread number: nt, number of elements: n, array: a
f64 reduc_parallel(f64 *restrict a, u64 n, u64 nt)
{
  //Reduction value
  f64 r = 0.0;

  //Mask for thread pinning
  cpu_set_t cpuset;
  
  //Allocating threads data structure
  thread_data_t *td = malloc(sizeof(thread_data_t) * nt);

  if (!td)
    {
      printf("Error: cannot allocate thread data\n");
      exit(-1);
    }

  //Creating and pinning threads
  //creation, allocation, distrubution of charges
  for (u64 i = 0; i < nt; i++)
    {
      //Clear cpuset mask (64 bits), initialized 0
      CPU_ZERO(&cpuset);
      
      //Setting up the target CPU core, thread i to core i
      CPU_SET(i, &cpuset);

      //Number of elements per thread. 
      //No load balancing! rest of the division: 1, 2, 3 
      td[i].n = (n / nt); //block size
      td[i].a = a + i * td[i].n; //pointer to the beginning of the block 
      td[i].r = 0.0; //partial sum, initialized 0
      
      //Create the thread
      pthread_create(&td[i].id, NULL, _reduc_, &td[i]); //&td[i] : parameter of _reduc_

      //Pin the thread on the previously set up core 
      pthread_setaffinity_np(td[i].id, sizeof(cpuset), &cpuset);

      //
      printf("thread id: %llu; core: %llu\n", (u64)td[i].id, i);
    } 
  
  //barrior: waiting 
  //clone: command to create a thread

  //Finilizing
  // partial sum -> sum
  for (u64 i = 0; i < nt; i++)
    {
      pthread_join(td[i].id, NULL);

      r += td[i].r;
    }
  //mutex : mutual exclusion
  //to realize atomicity here
  return r;
}

//
int main(int argc, char **argv)
{
  //
  if (argc < 3)
    return printf("Usage: %s [n] [t]\n", argv[0]), -1;

  u64 n = atoll(argv[1]);
  u64 nt = atoll(argv[2]);

  if (!n)
    {
      printf("Error: number of array elements is 0\n");
      exit(-1);
    }

  if (!nt)
    {
      printf("Error: number threads is 0\n");
      exit(-1);
    }

  //Size in bytes
  u64 s = n * sizeof(f64);
  
  //
  f64 *restrict a = aligned_alloc(32, s);

  init(a, n, 'c');
  
  f64 rs  = reduc_sequential(a, n);
  f64 rp  = reduc_parallel(a, n, nt);

  //
  printf("allocated memory : %llu B, %llu KiB, %llu MiB, %llu GiB\n",
	 s,
	 s >> 10,
	 s >> 20,
	 s >> 30);
  
  printf("sequential result: %lf\n", rs);
  printf("parallel result  : %lf\n", rp);

  f64 delta = fabs(rs - rp);
  
  //verify precision problem
  printf("delta            : %lf (%e)\n", delta, delta);
  
  //
  free(a);
  
  //
  return 0;
}
