//
#define _GNU_SOURCE

//
#include <omp.h>
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
      a[i] = (f64)RAND_MAX / (f64)rand(); //random number generated [0.0, 1.0)
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
  
  // Calling a function (reduc_sequential) with parameters from the thread_data_t structure
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
  
<<<<<<< HEAD:TP3/src/reduc_parallel.c
  //Allocating threads data structure
  thread_data_t *td = malloc(sizeof(thread_data_t) * nt);

=======
  //Allocating threads data array
  thread_data_t **td = malloc(sizeof(thread_data_t *) * nt);
  
>>>>>>> ee701ec (Updating TP3 with new codes):TP3/src/base/reduc_parallel.c
  if (!td)
    {
      printf("Error: cannot allocate thread data\n");
      exit(-1);
    }

  //
  f64 n_mod = (n % nt);
  f64 n_div = (n / nt);
  
  //Creating and pinning threads
  //creation, allocation, distrubution of charges
  for (u64 i = 0; i < nt; i++)
    {
<<<<<<< HEAD:TP3/src/reduc_parallel.c
      //Clear cpuset mask (64 bits), initialized 0
=======
      //Allocate thread data entry in the array
      td[i] = malloc(sizeof(thread_data_t));
      
      //Clear cpuset mask
>>>>>>> ee701ec (Updating TP3 with new codes):TP3/src/base/reduc_parallel.c
      CPU_ZERO(&cpuset);
      
      //Setting up the target CPU core, thread i to core i
      CPU_SET(i, &cpuset);

      //Number of elements per thread. 
<<<<<<< HEAD:TP3/src/reduc_parallel.c
      // add cases where n % nt != 0
      td[i].n = (n / nt) + (i < n % nt ? 1 : 0); //block size
      td[i].a = a + i * (n / nt + (i < n % nt ? 1 : 0)); //pointer to the beginning of the block 
      td[i].r = 0.0; //partial sum, initialized 0
      
      //Create the thread
      pthread_create(&td[i].id, NULL, _reduc_, &td[i]); //&td[i] : parameter of _reduc_
=======
      td[i]->n = n_div + (n_mod != 0);
      td[i]->a = a + (i * td[i]->n);
      td[i]->r = 0.0;
      
      //Create the thread
      pthread_create(&td[i]->id, NULL, _reduc_, td[i]);
>>>>>>> ee701ec (Updating TP3 with new codes):TP3/src/base/reduc_parallel.c

      //Pin the thread on the previously set up core 
      pthread_setaffinity_np(td[i]->id, sizeof(cpuset), &cpuset);

<<<<<<< HEAD:TP3/src/reduc_parallel.c
      //
      printf("thread id: %llu; core: %llu\n", (u64)td[i].id, i);
    } 
=======
      if (n_mod)
	n_mod--;
    }
>>>>>>> ee701ec (Updating TP3 with new codes):TP3/src/base/reduc_parallel.c
  
  //barrior: waiting 
  //clone: command to create a thread

  //Finilizing
  // partial sum -> sum
  for (u64 i = 0; i < nt; i++)
    {
      pthread_join(td[i]->id, NULL);

      r += td[i]->r;
      
      free(td[i]);
    }
<<<<<<< HEAD:TP3/src/reduc_parallel.c
  //mutex : mutual exclusion
  //to realize atomicity here
=======

  free(td);
  
>>>>>>> ee701ec (Updating TP3 with new codes):TP3/src/base/reduc_parallel.c
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
  printf("\nallocated memory  : %llu B, %llu KiB, %llu MiB, %llu GiB\n",
	 s,
	 s >> 10,
	 s >> 20,
	 s >> 30);

  //
  f64 t1 = 0.0, t2 = 0.0;
  
<<<<<<< HEAD:TP3/src/reduc_parallel.c
  //verify precision problem
  printf("delta            : %lf (%e)\n", delta, delta);
=======
  //
  f64 *restrict a = aligned_alloc(64, s);

  init(a, n, 'c');

  //Sequential
  t1 = omp_get_wtime();
  
  f64 rs  = reduc_sequential(a, n);

  t2 = omp_get_wtime();  

  f64 elapsed_s = (f64)(t2 - t1);

  //Parallel
  t1 = omp_get_wtime();
  
  f64 rp  = reduc_parallel(a, n, nt);

  t2 = omp_get_wtime();

  f64 elapsed_p = (f64)(t2 - t1);
    
  printf("\nsequential result : %lf\n", rs);
  printf("sequential elapsed: %.5lf s\n", elapsed_s);
  printf("\nparallel result   : %lf\n", rp);
  printf("parallel elapsed  : %.5lf s\n", elapsed_p);
  
  f64 delta = fabs(rs - rp);
  f64 speedup = (elapsed_s / elapsed_p);
  
  printf("\nresults delta      : %lf (%e)\n", delta, delta);
  printf("speedup           : %.3lf\n", speedup);
>>>>>>> ee701ec (Updating TP3 with new codes):TP3/src/base/reduc_parallel.c
  
  //
  free(a);
  
  //
  return 0;
}
