CC=gcc
CFLAGS=-Wall -g
OFLAGS=-O3 -fopenmp
#OpenMP allows you to parallelize loops, sections of code, 
#and other parts of your program to achieve better performance on multicore systems.
AFLAGS=-march=native -mtune=native
LFLAGS=-lpthread -lm

all: reduc_parallel reduc_parallel_simd

reduc_parallel: reduc_parallel.c
	$(CC) $(AFLAGS) $(CFLAGS) $(OFLAGS) $< -o $@ $(LFLAGS) 

reduc_parallel_simd: reduc_parallel_simd.c
	$(CC) $(AFLAGS) $(CFLAGS) $(OFLAGS) $< -o $@ $(LFLAGS) 

clean:
	rm -Rf reduc_parallel reduc_parallel_simd
