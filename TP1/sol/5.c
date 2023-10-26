//hamming distance between DNA chaines
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

//Defining error codes
#define ERR_FNAME_NULL   0
#define ERR_MALLOC_NULL  1
#define ERR_STAT         3
#define ERR_OPEN_FILE    4
#define ERR_READ_BYTES   5
#define ERR_NULL_POINTER 6

//
#define REPS 10000000

//
typedef unsigned char      u8;
typedef unsigned long long u64;

//Sequence definitions 
typedef struct {

  //Sequence elements/bytes
  u8 *bases;  //这是一个 u8 指针，用于存储序列的元素或字节数据。可以被视为一个指向字节数组的指针。

  //Sequence length
  u64 len;    //表示序列的长度，即序列中包含的元素或字节的数量。

} seq_t; //这是一个用户定义的结构体，用于表示序列数据。
//使用这些类型和结构体，您可以创建和操作序列数据，其中 seq_t 结构体包含了序列的元素和长度信息。
//这种抽象的表示使得处理序列数据更加灵活和可维护，可以方便地传递和操作序列数据。


//Global error variable
u64 err_id = 0;

//Error messages
const char *err_msg[] = {

  "file name pointer NULL",
  "memory allocation fail, 'malloc' returned NULL",
  "cannot 'stat' file",
  "cannot open file, 'fopen' returned NULL",
  "mismatch between read bytes and file length",
  
  NULL
};

//这是一个用于获取时间戳计数（TSC，Time Stamp Counter）值的内联汇编函数。
//TSC是一个CPU寄存器，通常用于度量CPU执行指令的时钟周期数，以测量程序执行时间和性能。
static inline u64 rdtsc()
{
  u64 a, d;  //声明两个64位无符号整数变量 a 和 d，用于存储TSC值的低32位和高32位。
  
  __asm__ volatile ("rdtsc" : "=a" (a), "=d" (d)); 
  //这是内联汇编代码，执行了RDTSC（Read Time-Stamp Counter）指令，该指令将TSC的值读入寄存器EAX和EDX。
  //内联汇编使用了扩展的内联汇编语法，其中 "rdtsc" 是汇编指令，"=a" 和 "=d" 指示将结果分别返回给变量 a 和 d。

  return ((d << 32) | a); //通过将 d 左移32位并与 a 进行位或操作，将获取的TSC值合并为一个64位的无符号整数，然后将其作为函数的返回值。
} //这个函数允许程序在需要测量执行时间或性能时获取TSC计数值。由于TSC的计数单位是CPU时钟周期，因此可以使用它来进行精确的时间测量。
  //TSC值在不同的CPU和系统上可能有不同的行为，因此在跨平台应用中需要小心处理。

//
void error()
{
  //打印错误消息，其中包括错误标识符 err_id 和与该错误标识符相关的错误消息 err_msg[err_id]。
  //这将在标准输出中显示错误的描述，以帮助程序员或用户理解错误的类型和原因。
  printf("Error (%llu): %s\n", err_id, err_msg[err_id]);
  
  //调用 exit 函数，终止程序的执行并返回退出状态 -1，表示程序因错误而非正常退出。
  //这将终止程序的执行并返回一个错误状态，以通知调用者或操作系统发生了错误。
  exit(-1);
} //在错误处理中，当某个函数或操作遇到错误情况时，通常会调用类似 error 函数来报告和处理错误。
  //这有助于提高程序的可维护性和可读性，因为错误处理的代码可以在一个地方进行集中管理，而不需要在多个地方分散处理错误。
  //同时，使用错误消息和错误标识符也可以更好地记录和诊断程序中发生的错误。

//
seq_t *load_seq(const char *fname)
{
  //首先检查传递给函数的文件名指针 fname 是否为 NULL。如果是 NULL，表示文件名未提供，
  //因此将错误标识符 err_id 设置为 ERR_FNAME_NULL，并返回 NULL 指针，表示出现了错误。
  if (!fname)
    {
      err_id = ERR_FNAME_NULL;
      return NULL;
    }

  //创建一个名为 sb 的结构体，用于存储文件状态信息，包括文件大小等。
  struct stat sb;

  //使用 stat 函数获取文件 fname 的状态信息，并将结果存储在 sb 结构体中。
  //如果 stat 函数返回小于0的值（通常是-1），表示获取状态信息失败，可能是因为文件不存在或其他问题。
  //在这种情况下，将错误标识符 err_id 设置为 ERR_STAT，并返回 NULL 指针，表示出现了错误。
  if (stat(fname, &sb) < 0)
    {
      err_id = ERR_STAT;
      return NULL;
    }
  
  //Allocate sequence 
  //在这里，为存储序列数据的 seq_t 结构体分配内存，并将指针存储在 s 中。
  //如果内存分配失败，将错误标识符 err_id 设置为 ERR_MALLOC_NULL，并返回 NULL 指针，表示出现了错误。
  seq_t *s = malloc(sizeof(seq_t));
  
  if (!s)
    {
      err_id = ERR_MALLOC_NULL;
      return NULL;
    }
  
  //Length of sequence is file size in bytes
  //将结构体 seq_t 中的 s->len 成员设置为 sb.st_size，其中 sb.st_size 是一个表示文件大小的变量，通常是通过文件状态信息（stat）获得的。
  //这行代码将文件的大小分配给序列结构体中的 len 成员，以表示序列的长度。
  s->len = sb.st_size; 

  //Allocating memory for sequence bases
  //为存储序列元素或字节数据的数组 s->bases 分配内存。分配的内存大小是 sb.st_size 乘以 sizeof(u8)，即文件大小乘以一个字节的大小。
  //这将创建一个能够容纳整个序列的内存块，并将指向该内存块的指针存储在 s->bases 成员中。
  s->bases = malloc(sizeof(u8) * sb.st_size);
  
  if (!s->bases) //在动态内存分配失败时执行以下操作。
    {
      err_id = ERR_MALLOC_NULL;  //将错误标识符 err_id 设置为 ERR_MALLOC_NULL，表示内存分配失败的错误类型。
      return NULL;  //返回 NULL 指针，表示内存分配失败，可能需要进行错误处理。
    } //这段代码的主要目的是为表示序列数据的结构体 seq_t 分配内存，并将文件大小分配给序列的长度 len，
      //以便可以在这个内存块中存储序列的元素。如果内存分配失败，它会设置错误标识符，并返回 NULL 指针以指示错误状态。
      //这是在处理从文件读取的序列数据时执行的初始化操作。

  //Opening the file
  //使用 fopen 函数以只读二进制模式打开指定文件 fname。
  //如果打开文件失败（fp 为 NULL），则将错误标识符 err_id 设置为 ERR_OPEN_FILE，表示无法打开文件，然后返回 NULL 指针，表示出现了错误。
  FILE *fp = fopen(fname, "rb");

  if (!fp)
    {
      err_id = ERR_OPEN_FILE;
      return NULL;
    }

  //Reading bytes from file
  //使用 fread 函数从已经打开的文件中读取数据，读取的数据存储在 s->bases 中。s->len 表示要读取的元素数量，sizeof(u8) 表示每个元素的大小。
  //read_bytes 存储了实际读取的字节数。如果读取失败，read_bytes 的值可能小于 s->len。
  size_t read_bytes = fread(s->bases, sizeof(u8), s->len, fp);

  //Closing file
  //使用 fclose 函数关闭已经打开的文件，释放文件资源。
  fclose(fp);

  //Check if bytes were fully read
  //检查是否读取的字节数等于 s->len，如果不相等，将错误标识符 err_id 设置为 ERR_READ_BYTES，表示读取的字节数与序列的长度不匹配，
  //然后返回 NULL 指针，表示出现了错误。
  if (read_bytes != s->len)
    {
      err_id = ERR_READ_BYTES;
      return NULL;
    }
  
  //如果一切正常，s 将包含加载的序列数据。
  return s;
}

//释放 seq_t 结构体及其关联的内存
void release_seq(seq_t *s)
{
  //检查传递给函数的 seq_t 结构体指针 s 是否为非空
  //确保不会对空指针执行释放操作
  if (s)
    {
      //如果 seq_t 结构体中的 s->bases 成员（存储序列数据的指针）非空，则执行以下操作：
      if (s->bases)  
	free(s->bases);  //释放 s->bases 指向的内存块，这是为存储序列数据而分配的内存。
      else
	err_id = ERR_NULL_POINTER;  //将错误标识符 err_id 设置为 ERR_NULL_POINTER，表示尝试释放一个空指针。
	  
      //将 s->len 设置为0，表示序列长度为0
      s->len = 0;
    }
  else
    err_id = ERR_NULL_POINTER; //如果 seq_t 结构体指针 s 为空，则将错误标识符 err_id 设置为 ERR_NULL_POINTER，表示尝试释放一个空指针。
} //这个函数用于在释放 seq_t 结构体及其关联的内存时进行安全的操作。
  //它会检查指针的有效性，释放内存并清除相关数据，同时记录任何可能的错误情况。
  //这有助于避免释放无效的内存或空指针，并提高程序的稳定性。

//接受两个字节数组指针 a 和 b，以及一个无符号64位整数 n，表示要比较的字节数目。函数返回一个无符号64位整数，表示汉明距离。
u64 hamming_c(u8 *a, u8 *b, u64 n)
{
  //声明并初始化一个无符号64位整数 h 为0，用于存储汉明距离的计数。
  u64 h = 0;

  //使用 for 循环遍历 a 和 b 数组中的每个字节，i 从0递增到 n-1。
  //在循环中，对每对相同位置上的字节执行以下操作：
  for (u64 i = 0; i < n; i++)
    h += __builtin_popcount(a[i] ^ b[i]); 
  //a[i] ^ b[i] 计算 a 和 b 在相同位置上的字节的异或结果，这将标识出不同字节的位。
  //__builtin_popcount 是一个内置函数，它用于计算整数中设置为1的位的数量，这里用于计算不同字节的位数。
  //将结果添加到 h 中，以累积汉明距离

  //返回计算得到的汉明距离，即两个字节数组之间的不同字节的位数之和。
  return h;
} //这个函数使用循环遍历两个数组中的每个字节，计算它们之间的不同位数，最终返回汉明距离。
  //这是一种常用的方法来衡量两个二进制序列之间的差异。

//这段代码实现了一个名为 hamming_asm_8bits 的汉明距离计算函数，使用汇编语言来执行
//这是函数的声明，接受两个字节数组指针 a 和 b，以及一个无符号64位整数 n，表示要比较的字节数目。
//函数返回一个无符号64位整数，表示汉明距离。
u64 hamming_asm_8bits(u8 *a, u8 *b, u64 n)
{
  //在函数开始时，声明并初始化一个无符号64位整数 h 为0，用于存储汉明距离的计数。
  u64 h = 0;

  //Size in bytes
  //将 sizeb 初始化为 n，表示要比较的字节数目。
  u64 sizeb = n;
  
  //汇编代码块：在这个块中，使用汇编语言来执行汉明距离的计算。这个块包括一系列汇编指令，用于循环遍历字节数组 a 和 b 并计算汉明距离。
  __asm__ volatile (
        //xor 指令用于清零寄存器
		    "xor %%rax, %%rax;\n"  
		    "xor %%rbx, %%rbx;\n"
		    "xor %%rcx, %%rcx;\n"
		    "xor %%rdx, %%rdx;\n"

        //1:; 是一个标签，用于表示循环的入口点
		    "1:;\n" 

		    //Loading bytes   
        //movb 指令用于加载字节值     
		    "movb (%[_a], %%rcx), %%al;\n" 
		    "movb (%[_b], %%rcx), %%bl;\n"

		    //Xoring bytes    
        //xor 指令用于执行异或操作
		    "xor %%bl, %%al;\n"

		    //Popcount on 16bits (no 8 bits popcount on x86)     
        //popcnt 指令用于计算汉明距离的16位部分
		    "popcnt %%ax, %%bx;\n"
		    
        //add 指令用于累积汉明距离
		    "add %%rbx, %%rdx;\n"  
		    
        //add 指令用于递增循环计数器
		    "add $1, %%rcx;\n"   
        //add 指令用于递增循环计数器  
		    "cmp %[_sizeb], %%rcx;\n"   
        //jl 指令用于条件跳转，如果计数器小于 sizeb，则继续循环
		    "jl 1b;\n"          
		    
        //将计算得到的汉明距离存储在寄存器 rdx 中的值传递给输出变量 h
		    "mov %%rdx, %[_h];\n"    
		    
		    : //outputs 
		      [_h] "=r" (h)
		      
		    : //inputs
		      [_a]     "r" (a),
		      [_b]     "r" (b),
		      [_sizeb] "r" (sizeb)
		      
		    :
		    "cc", "memory", "rax", "rbx", "rcx", "rdx");
  
  //返回计算得到的汉明距离 h
  return h;
} //这个函数使用汇编语言来执行汉明距离的计算，通过循环遍历字节数组 a 和 b 中的每个字节，执行异或和汉明距离计算操作，
  //最终返回汉明距离。这种方法可以在汇编级别更有效地执行计算。

//这个版本处理64位块而不是字节，从而提高了效率
u64 hamming_asm_64bits(u8 *a, u8 *b, u64 n)
{
  //
  u64 h = 0;

  //Size in bytes
  u64 sizeb = n;
  
  //
  __asm__ volatile (
		    "xor %%rcx, %%rcx;\n"
		    "xor %%rdx, %%rdx;\n"

		    "1:;\n"

		    //Loading 64bit blocks rather than bytes
        //movq 指令用于加载64位块的值
		    "movq (%[_a], %%rcx), %%rax;\n" 
		    "movq (%[_b], %%rcx), %%rbx;\n"

		    //
		    "xor %%rbx, %%rax;\n"
		    "popcnt %%rax, %%rbx;\n"

		    "add %%rbx, %%rdx;\n"
		    
		    "add $8, %%rcx;\n"
		    "cmp %[_sizeb], %%rcx;\n"
		    "jl 1b;\n"

		    "mov %%rdx, %[_h];\n"
		    
		    : //outputs
		      [_h] "=r" (h)
		      
		    : //inputs
		      [_a]     "r" (a),
		      [_b]     "r" (b),
		      [_sizeb] "r" (sizeb)
		      
		    :
		    "cc", "memory", "rax", "rbx", "rcx", "rdx");
  
  //
  return h;
} //这个版本的函数相比前一个版本处理的数据块更大，因此在处理大量数据时可能会更快。
  //它的原理与前一个版本类似，只是每次处理64位块而不是单个字节。这种方法可以在汇编级别更有效地执行计算。

//比较不同方法计算两个序列之间的汉明距离
int main(int argc, char **argv)
{
  //检查命令行参数是否足够，需要提供两个序列文件
  if (argc < 3)
    return printf("Usage: %s [seq1] [seq2]\n", argv[0]), 1;
  
  //Loading first sequence
  //加载第一个序列，使用 load_seq 函数从文件中读取序列数据
  seq_t *s1 = load_seq(argv[1]);

  if (!s1)
    error();

  //Loading second sequence
  //加载第二个序列，使用 load_seq 函数从文件中读取序列数据
  seq_t *s2 = load_seq(argv[2]);

  if (!s2)
    error();
  
  //Check size
  //检查两个序列的长度是否相等，因为汉明距离需要两个等长的序列
  if (s1->len != s2->len)
    return printf("Error: sequences must match in length"), 2;
  
  //存储不同方法计算得到的汉明距离
  u64 h_c     = 0; 
  u64 h_asm8  = 0; 
  u64 h_asm64 = 0; 

  //定义了变量 a 和 b 来存储时间戳，以及 e_c、e_asm8 和 e_asm64 来存储执行计算的时间。
  u64 a = 0, b = 0;
  u64 e_c = 0, e_asm8 = 0, e_asm64 = 0;
  
  //C
  //使用 rdtsc 函数获取时间戳
  b = rdtsc();
  
  //使用循环 for (u64 i = 0; i < REPS; i++) 来重复计算汉明距离，其中 REPS 表示重复次数。
  //计算不同方法的执行时间差并存储在 e_c、e_asm8 和 e_asm64 中
  for (u64 i = 0; i < REPS; i++)
    h_c     = hamming_c(s1->bases, s2->bases, s1->len);

  a = rdtsc();
  e_c = a - b;

  //ASM 8 bits
  b = rdtsc();
  
  for (u64 i = 0; i < REPS; i++)
    h_asm8 = hamming_asm_8bits(s1->bases, s2->bases, s1->len);

  a = rdtsc();
  e_asm8 = a - b;

  
  //ASM 8 bits
  b = rdtsc();
  
  for (u64 i = 0; i < REPS; i++)
    h_asm64 = hamming_asm_64bits(s1->bases, s2->bases, s1->len);

  a = rdtsc();
  e_asm64 = a - b;
  
  //打印每种方法计算的汉明距离和平均循环次数。
  printf("hamming_c    : %llu, cycles: %llu\n", h_c    , e_c     / REPS);
  printf("hamming_asm8 : %llu, cycles: %llu\n", h_asm8 , e_asm8  / REPS);
  printf("hamming_asm64: %llu, cycles: %llu\n", h_asm64, e_asm64 / REPS);
  
  //释放内存并释放序列数据。
  release_seq(s1); free(s1);
  release_seq(s2); free(s2);
  
  //
  return 0;
}  //这个程序用于比较 C 语言版本的汉明距离计算函数和两个汇编版本的汉明距离计算函数的性能。
  //通过测量执行时间和循环次数，可以评估不同方法的性能和效率。
