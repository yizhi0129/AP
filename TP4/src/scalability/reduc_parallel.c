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
  pthread_t id;
  
  //Number of elements handled by a thread
  u64 n;

  //Array of elements
  f64 *restrict a;

  //Partial sum
  f64 r;
  
} thread_data_t;

//
void init(f64 *restrict a, u64 n, u8 type)
{
  srand(0);
  
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

//
f64 reduc_sequential(f64 *restrict a, u64 n)
{
  f64 r = 0.0;
  
  for (u64 i = 0; i < n; i++)
    r += a[i];
  
  return r;
}

//
f64 reduc_openmp(f64 *restrict a, u64 n)
{
  f64 r = 0.0;
  
#pragma omp parallel for reduction (+:r)
  for (u64 i = 0; i < n; i++)
    r += a[i];
  
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

  //
  f64 t1 = 0.0, t2 = 0.0;
  
  //Size in bytes
  u64 s = n * sizeof(f64);

  //
  printf("\nallocated memory  : %llu B, %llu KiB, %llu MiB, %llu GiB\n",
	 s,
	 s >> 10,
	 s >> 20,
	 s >> 30);
  
  //
  f64 result      = 0.0;
  f64 elapsed     = 0.0;
  
  //
  f64 *times     = malloc(sizeof(f64) * nt);

  if (!times)
    {
      printf("Error: cannot allocate micros array\n");
      exit(-1);
    }
  
  f64 *reductions = malloc(sizeof(f64) * nt);

  if (!reductions)
    {
      printf("Error: cannot allocate reductions array\n");
      exit(-1);
    }

  //Running scalability measurements
  for (u64 i = 0; i < nt; i++)
    {      
      //
      f64 *restrict a = aligned_alloc(64, s);
      
      init(a, n, 'c');
      
      omp_set_num_threads(i + 1);
      
      //
      do
	{
	  t1 = omp_get_wtime();
	  
	  result = reduc_openmp(a, n);
	  
	  t2 = omp_get_wtime();
	  
	  elapsed = (f64)(t2 - t1);
	}
      while (elapsed <= 0.0);
      
      //
      times[i]      = elapsed;
      reductions[i] = result;
      
      //
      free(a);
      
      printf("# threads: %3llu; # elements/thread: %18llu; elapsed: %13.5lf s; result: %lf\n",
	     i + 1,
	     n / (i + 1),
	     times[i],
	     reductions[i]);
    }

  //
  for (u64 i = 1; i < nt; i++)
    {
      f64 delta = fabs(reductions[0] - reductions[i]);
      f64 speedup = times[0] / times[i];
      
      printf("delta(0, %llu) = %lf (%e); speedup: %.3lf\n", i, delta, delta, speedup);
    }
  
  //
  free(times);
  free(reductions);
  
  //
  return 0;
}
