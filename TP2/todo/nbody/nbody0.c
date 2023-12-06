/*
  N-BODY collision simulation
  
  Bad code --> optimize
  
*/

#include <time.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>

//define a structure of vectors
typedef struct {

  double x, y;
  
} vector;

//including w and h for the width and height of the simulation space
int w, h;

//
int nbodies, timeSteps;

//
double *masses, GravConstant;

//
vector *positions, *velocities, *accelerations;

//This function measures the number of clock cycles using the rdtsc instruction, providing a way to time code execution.
unsigned long long rdtsc(void)
{
  unsigned long long a, d;
  
  __asm__ volatile ("rdtsc" : "=a" (a), "=d" (d));
  
  return (d << 32) | a;
}

//This function generates a random integer within a given range (x to y) using the rand() function.
int randxy(int x, int y)
{
  return (rand() % (y - x + 1)) + x; 
}

//This function generates a random floating-point number between -1 and 1.
double randreal()
{
  int s = (randxy(0, 1)) ? 1 : -1;
  int a = randxy(1, RAND_MAX), b = randxy(1, RAND_MAX);

  return s * ((double)a / (double)b); 
}

//
vector add_vectors_c(vector a, vector b)
{
  vector c = { a.x + b.x, a.y + b.y }; //add the x components together and the y components together
  
  return c;
}


<<<<<<< HEAD
vector add_vectors_asm1(vector a, vector b)
{
  vector c;
  __asm__ volatile (
=======
vector add_vectors_sse(vector a, vector b)
{
  vector c;
  __asm__ volatile 
  (
>>>>>>> new
    "movss %[a_x], %%xmm0\n"
    "movss %[b_x], %%xmm1\n"
    "addss %%xmm1, %%xmm0\n"
    "movss %%xmm0, %[c_x]\n"
    "movss %[a_y], %%xmm0\n"
    "movss %[b_y], %%xmm1\n"
    "addss %%xmm1, %%xmm0\n"
    "movss %%xmm0, %[c_y]\n"
    : [c_x] "=m" (c.x), [c_y] "=m" (c.y)
    : [a_x] "m" (a.x), [a_y] "m" (a.y), [b_x] "m" (b.x), [b_y] "m" (b.y)
    : "xmm0", "xmm1"
  );
  return c;
} 

<<<<<<< HEAD
=======
vector add_vectors_avx2(vector a, vector b) 
{
  vector c;

    // Load 'a' and 'b' into AVX2 registers
  __m256d va = _mm256_set_pd(a.y, a.x, a.y, a.x);
  __m256d vb = _mm256_set_pd(b.y, b.x, b.y, b.x);

    // Add the elements of 'va' and 'vb'
  __m256d result = _mm256_add_pd(va, vb);

    // Store the result back to 'c'
  _mm256_storeu_pd(&c.x, result);

  return c;
}

vector add_vectors_avx512(vector a, vector b) 
{
  vector c;

    // Load 'a' and 'b' into AVX512 registers
  __m512d va = _mm512_set_pd(a.y, a.x, a.y, a.x, a.y, a.x, a.y, a.x);
  __m512d vb = _mm512_set_pd(b.y, b.x, b.y, b.x, b.y, b.x, b.y, b.x);

    // Add the elements of 'va' and 'vb'
  __m512d result = _mm512_add_pd(va, vb);

    // Store the result back to 'c'
  _mm512_storeu_pd(&c.x, result);

  return c;
}


>>>>>>> new

//
vector scale_vector_c(double b, vector a)
{
  vector c = { b * a.x, b * a.y };
  
  return c;
}


<<<<<<< HEAD
vector scale_vector_asm1(double b, vector a)
{
  vector c;
  __asm__ volatile (
        "movsd %[_b], %%xmm0;\n"  
        "movsd %[_a_x], %%xmm1;\n"  
        "movsd %[_a_y], %%xmm2;\n"  
        "mulsd %%xmm1, %%xmm0;\n"  
        "mulsd %%xmm2, %%xmm0;\n"   
        "movsd %%xmm0, %[_c_x];\n"  
        "movsd %%xmm0, %[_c_y];\n"  
        : [_c_x] "=m" (c.x), [_c_y] "=m" (c.y)
        : [_b] "m" (b), [_a_x] "m" (a.x), [_a_y] "m" (a.y)
        : "xmm0", "xmm1", "xmm2", "cc", "memory");
=======
vector scale_vector_sse(double b, vector a)
{
  vector c;
  __asm__ volatile 
  (
    "movsd %[_b], %%xmm0;\n"  
    "movsd %[_a_x], %%xmm1;\n"  
    "movsd %[_a_y], %%xmm2;\n"  
    "mulsd %%xmm1, %%xmm0;\n"  
    "mulsd %%xmm2, %%xmm0;\n"   
    "movsd %%xmm0, %[_c_x];\n"  
    "movsd %%xmm0, %[_c_y];\n"  
    : [_c_x] "=m" (c.x), [_c_y] "=m" (c.y)
    : [_b] "m" (b), [_a_x] "m" (a.x), [_a_y] "m" (a.y)
    : "xmm0", "xmm1", "xmm2", "cc", "memory"
  );
  return c;
}

vector scale_vector_avx2(double b, vector a)
{
  vector c;
  __m256d vb = _mm256_set1_pd(b);  // Broadcast 'b' to all elements of a AVX2 register

    // Load 'a' into a single AVX2 register
  __m256d va = _mm256_set_pd(a.y, a.x, a.y, a.x);

    // Multiply the elements of 'va' and 'vb'
  __m256d result = _mm256_mul_pd(va, vb);

    // Store the result back to 'c'
  _mm256_storeu_pd(&c.x, result);

  return c;
}

vector scale_vector_avx512(double b, vector a) 
{
  vector c;

    // Load 'b' into AVX512 register
  __m512d vb = _mm512_set1_pd(b);

    // Load 'a' into AVX512 registers
  __m512d va = _mm512_set_pd(a.y, a.x, a.y, a.x, a.y, a.x, a.y, a.x);

    // Multiply the elements of 'va' and 'vb'
  __m512d result = _mm512_mul_pd(va, vb);

    // Store the result back to 'c'
  _mm512_storeu_pd(&c.x, result);

>>>>>>> new
  return c;
}



//
vector sub_vectors_c(vector a, vector b)
{
  vector c = { a.x - b.x, a.y - b.y };
  
  return c;
}

<<<<<<< HEAD
vector sub_vectors_asm1(vector a, vector b) 
{
    vector c;
    
    __asm__ volatile (
        "movsd %[a_x], %%xmm0;\n"   
        "subsd %[b_x], %%xmm0;\n"   
        "movsd %%xmm0, %[c_x];\n"   

        "movsd %[a_y], %%xmm1;\n"   
        "subsd %[b_y], %%xmm1;\n"   
        "movsd %%xmm1, %[c_y];\n"   
        
        : [c_x] "=m" (c.x), [c_y] "=m" (c.y)  
        : [a_x] "m" (a.x), [a_y] "m" (a.y), [b_x] "m" (b.x), [b_y] "m" (b.y)  
        : "xmm0", "xmm1"  
    );

    return c;
=======
vector sub_vectors_sse(vector a, vector b) 
{
  vector c;
    
  __asm__ volatile 
  (
    "movsd %[a_x], %%xmm0;\n"   
    "subsd %[b_x], %%xmm0;\n"   
    "movsd %%xmm0, %[c_x];\n"   

    "movsd %[a_y], %%xmm1;\n"   
    "subsd %[b_y], %%xmm1;\n"   
    "movsd %%xmm1, %[c_y];\n"   
        
    : [c_x] "=m" (c.x), [c_y] "=m" (c.y)  
    : [a_x] "m" (a.x), [a_y] "m" (a.y), [b_x] "m" (b.x), [b_y] "m" (b.y)  
    : "xmm0", "xmm1"  
  );

  return c;
}


vector sub_vectors_avx2(vector a, vector b) 
{
  vector c;

    // Load 'a' and 'b' into AVX2 registers
  __m256d va = _mm256_set_pd(a.y, a.x, a.y, a.x);
  __m256d vb = _mm256_set_pd(b.y, b.x, b.y, b.x);

    // Subtract the elements of 'vb' from 'va'
  __m256d result = _mm256_sub_pd(va, vb);

    // Store the result back to 'c'
  _mm256_storeu_pd(&c.x, result);

  return c;
}

vector sub_vectors_avx512(vector a, vector b) 
{
  vector c;

    // Load 'a' and 'b' into AVX512 registers
  __m512d va = _mm512_set_pd(a.y, a.x, a.y, a.x, a.y, a.x, a.y, a.x);
  __m512d vb = _mm512_set_pd(b.y, b.x, b.y, b.x, b.y, b.x, b.y, b.x);

    // Subtract the elements of 'vb' from 'va'
  __m512d result = _mm512_sub_pd(va, vb);

    // Store the result back to 'c'
  _mm512_storeu_pd(&c.x, result);

  return c;
>>>>>>> new
}

//
double mod_c(vector a)
{
  return sqrt(a.x * a.x + a.y * a.y);
}


<<<<<<< HEAD
double mod_asm1(vector a) {
    double c;

    __asm__ volatile (
        "movsd %[a_x], %%xmm0;\n"   
        "mulsd %%xmm0, %%xmm0;\n"   
        "movsd %[a_y], %%xmm1;\n"   
        "mulsd %%xmm1, %%xmm1;\n"  
        "addsd %%xmm0, %%xmm1;\n"  
        "sqrtsd %%xmm1, %[c];\n"   

        : [c] "=m" (c)   
        : [a_x] "m" (a.x), [a_y] "m" (a.y)   
        : "xmm0", "xmm1"   
    );

    return c;
}


=======
double mod_sse(vector a) 
{
  double c;

  __asm__ volatile 
  (
      "movsd %[a_x], %%xmm0;\n"   
      "mulsd %%xmm0, %%xmm0;\n"   
      "movsd %[a_y], %%xmm1;\n"   
      "mulsd %%xmm1, %%xmm1;\n"  
      "addsd %%xmm0, %%xmm1;\n"  
      "sqrtsd %%xmm1, %[c];\n"   

      : [c] "=m" (c)   
      : [a_x] "m" (a.x), [a_y] "m" (a.y)   
      : "xmm0", "xmm1"   
  );

  return c;
}

double mod_avx2(vector a) 
{
  double c;

    // Load 'a' into AVX2 register
  __m256d va = _mm256_set_pd(a.y, a.x, a.y, a.x);

    // Multiply the elements of 'va'
  __m256d result = _mm256_mul_pd(va, va);

    // Horizontal add across the register
  result = _mm256_hadd_pd(result, result);

    // Horizontal add again to get the sum of squares
  result = _mm256_hadd_pd(result, result);

    // Extract the square root
  _mm256_storeu_pd(&c, _mm256_sqrt_pd(result));

  return c;
}

double mod_avx512(vector a) 
{
  double c;

    // Load 'a' into AVX512 register
  __m512d va = _mm512_set_pd(a.y, a.x, a.y, a.x, a.y, a.x, a.y, a.x);

    // Multiply the elements of 'va'
  __m512d result = _mm512_mul_pd(va, va);

    // Horizontal add across the register
  result = _mm512_add_pd(result, _mm512_permutex_pd(result, 0x4E));

    // Horizontal add again to get the sum of squares
  result = _mm512_add_pd(result, _mm512_permutex_pd(result, 0xB1));

    // Extract the square root
  _mm512_storeu_pd(&c, _mm512_sqrt_pd(result));

  return c;
}
>>>>>>> new

//This function initializes the simulation parameters, including the simulation dimensions, number of particles, 
//gravitational constant, time steps, and arrays to store particle data. 
//It also generates initial random positions and velocities for the particles.
void init_system()
{
  w = h = 800;
  nbodies = 500;
  GravConstant = 1;
  timeSteps = 1000;
  
  //
  masses        = malloc(nbodies * sizeof(double));
  positions     = malloc(nbodies * sizeof(vector));
  velocities    = malloc(nbodies * sizeof(vector));
  accelerations = malloc(nbodies * sizeof(vector));

  //
  for (int i = 0; i < nbodies; i++)
    {
      masses[i] = 5;
      
      positions[i].x = randxy(10, w);
      positions[i].y = randxy(10, h);
      
      velocities[i].x = randreal();
      velocities[i].y = randreal();
    }
}

//This function resolves collisions between particles by swapping their velocities if two particles occupy the same position.
void resolve_collisions()
{
  //
  for (int i = 0; i < nbodies - 1; i++)
    for (int j = i + 1; j < nbodies; j++)
      if (positions[i].x == positions[j].x && positions[i].y == positions[j].y)
	    {
	      vector temp = velocities[i];
	      velocities[i] = velocities[j];
	      velocities[j] = temp;
	    }
}

//This function calculates the accelerations of all particles based on the gravitational interactions between them.
void compute_accelerations()
{ 
  for (int i = 0; i < nbodies; i++)
    {
      accelerations[i].x = 0;
      accelerations[i].y = 0;
      
      for(int j = 0; j < nbodies; j++)
	if(i != j)
	  accelerations[i] = add_vectors(accelerations[i],
					 scale_vector(GravConstant * masses[j] / (pow(mod(sub_vectors(positions[i], positions[j])), 3) + 1e7),
						      sub_vectors(positions[j], positions[i])));
    }
}

//This function updates the velocities of all particles based on the calculated accelerations.
void compute_velocities()
{  
  for (int i = 0; i < nbodies; i++)
    velocities[i] = add_vectors(velocities[i], accelerations[i]);
}

//This function updates the positions of all particles based on their current positions, velocities, and accelerations.
void compute_positions()
{
  for (int i = 0; i < nbodies; i++)
    positions[i] = add_vectors(positions[i], add_vectors(velocities[i], scale_vector(0.5, accelerations[i])));
}

//This function brings together the calculations for acceleration, velocity, and position updates and resolves collisions, 
//effectively simulating the behavior of the particles for one time step.
void simulate()
{
  compute_accelerations();
  compute_positions();
  compute_velocities();
  resolve_collisions();
}

//This is the entry point of the program. 
//It initializes SDL, creates a window and renderer for graphics, and calls init_system() to set up the simulation. 
//It then enters a main loop, where it repeatedly calls simulate(), updates the graphics, and handles user input to exit the simulation. 
//It also measures and prints the time taken for each time step and frame.
int main(int argc, char **argv)
{
  //
  int with_graphics = 0;
  
  if (argc == 2)
    {
      if (!strncmp(argv[1], "--with-graphics", 15))
	with_graphics = 1;
      else
	with_graphics = 0;
    }
  
  //
  int i;
  unsigned char quit = 0;
  SDL_Event event;
  SDL_Window *window;
  SDL_Renderer *renderer;
  
  srand(time(NULL));
  
  //
  if (with_graphics)
    {
      SDL_Init(SDL_INIT_VIDEO);
      SDL_CreateWindowAndRenderer(800, 800, SDL_WINDOW_OPENGL, &window, &renderer);
    }
  
  //
  init_system();
  
  //Main loop
  for (int i = 0; !quit && i < timeSteps; i++)
    {	  
      //
      double before = (double)rdtsc();
      
      simulate();

      //
      double after = (double)rdtsc();
      
      //
      printf("%d %lf\n", i, (after - before));

      if (with_graphics)
	{
	  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	  SDL_RenderClear(renderer);

	  for (int i = 0; i < nbodies; i++)
	    {
	      SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	      SDL_RenderDrawPoint(renderer, positions[i].x, positions[i].y);
	    }
      
	  SDL_RenderPresent(renderer);
	  
	  SDL_Delay(10);
	  
	  while (SDL_PollEvent(&event))
	    if (event.type == SDL_QUIT)
	      quit = 1;
	    else
	      if (event.type == SDL_KEYDOWN)
		if (event.key.keysym.sym == SDLK_q)
		  quit = 1;
	}
    }

  if (with_graphics)
    {
      SDL_DestroyRenderer(renderer);
      SDL_DestroyWindow(window);
      SDL_Quit();
    }
  
  return 0;
}
