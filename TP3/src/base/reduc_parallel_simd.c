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
f64 reduc_AVX(f64 *restrict a, u64 n)
{
  u64 s = sizeof(f64) * (n - (n & 3));
  f64 r[4] __attribute__((aligned(64)));

  __asm__ volatile (
		    "xor %%rcx, %%rcx;\n"
		    
		    "vxorpd %%ymm0, %%ymm0, %%ymm0;\n"
		    "vxorpd %%ymm1, %%ymm1, %%ymm1;\n"
		    "vxorpd %%ymm2, %%ymm2, %%ymm2;\n"
		    "vxorpd %%ymm3, %%ymm3, %%ymm3;\n"
		    
		    "1:;\n"
		    "vaddpd   (%[_a], %%rcx), %%ymm0, %%ymm0;\n"
		    "vaddpd 32(%[_a], %%rcx), %%ymm1, %%ymm1;\n"
		    "vaddpd 64(%[_a], %%rcx), %%ymm2, %%ymm2;\n"
		    "vaddpd 96(%[_a], %%rcx), %%ymm3, %%ymm3;\n"
		    
		    "add $128, %%rcx;\n"
		    "cmp %[_s], %%rcx;\n"
		    "jl 1b;\n"
		    
		    "vaddpd %%ymm1, %%ymm0, %%ymm0;\n"
		    "vaddpd %%ymm3, %%ymm2, %%ymm2;\n"
		    "vaddpd %%ymm2, %%ymm0, %%ymm0;\n"

		    "vmovapd %%ymm0, (%[_r]);\n"
		    
		    :
		    
		    :
		    [_a] "r" (a),
		    [_s] "r" (s),
		    [_r] "r" (r)
		    
		    :
		    "cc", "memory", "rcx",
		    "ymm0", "ymm1", "ymm2", "ymm3");

  for (u64 i = (n - (n & 3)); i < n; i++)
    r[0] += a[i];
  
  return (r[0] + r[1] + r[2] + r[3]);
}

//
void *_reduc_(void *p)
{
  thread_data_t *td = (thread_data_t *)p;
  
  td->r = reduc_AVX(td->a, td->n);
  
  return NULL;
}

//
f64 reduc_parallel(f64 *restrict a, u64 n, u64 nt)
{
  //Reduction value
  f64 r = 0.0;

  //Mask for thread pinning
  cpu_set_t cpuset;
  
  //Allocating threads data structure
  thread_data_t **td = malloc(sizeof(thread_data_t *) * nt);

  if (!td)
    {
      printf("Error: cannot allocate thread data\n");
      exit(-1);
    }

  //
  f64 n_mod = (n % nt);
  f64 n_div = (n / nt);
  
  //Creating and pinning threads
  for (u64 i = 0; i < nt; i++)
    {
      td[i] = malloc(sizeof(thread_data_t));
      
      //Clear cpuset mask
      CPU_ZERO(&cpuset);
      
      //Setting up the target CPU core
      CPU_SET(i, &cpuset);
      
      //Number of elements per thread. 
      td[i]->n = n_div + (n_mod != 0);
      td[i]->a = a + (i * td[i]->n);      
      td[i]->r = 0.0;
      
      //Create the thread
      pthread_create(&td[i]->id, NULL, _reduc_, td[i]);
      
      //Pin the thread on the previously set up core 
      pthread_setaffinity_np(td[i]->id, sizeof(cpuset), &cpuset);
      
      if (n_mod)
	n_mod--;
    }
  
  //Finilizing
  for (u64 i = 0; i < nt; i++)
    {
      pthread_join(td[i]->id, NULL);
      
      r += td[i]->r;

      free(td[i]);
    }

  free(td);
  
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
  printf("sequential elapsed: %.3lf s\n", elapsed_s);
  printf("\nparallel result   : %lf\n", rp);
  printf("parallel elapsed  : %.3lf s\n", elapsed_p);
  
  f64 delta = fabs(rs - rp);
  f64 speedup = (elapsed_s / elapsed_p);
  
  printf("\nresults delta      : %lf (%e)\n", delta, delta);
  printf("speedup           : %.3lf\n", speedup);
  
  //
  free(a);
  
  //
  return 0;
}
