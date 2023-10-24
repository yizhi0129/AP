//
#include <stdio.h>
#include <stdlib.h>

//函数接受三个参数：整数指针 p，表示要缩放的整数数组；无符号长整数 n，表示数组的大小；整数 s，表示缩放因子。
void scale_c(int *p, unsigned long long n, int s)
{
  //使用 for 循环遍历数组中的每个元素，循环变量 i 从0递增到 n-1。
  for (unsigned long long i = 0; i < n; i++)
    p[i] *= s; //在循环中，对数组中的每个元素 p[i] 进行缩放操作，即将其乘以整数 s，实现了整数数组的缩放。
} //这个函数的实现非常简单，但对于较大的数组，性能可能不如使用内联汇编的版本（scale_asm），
  //因为内联汇编版本可以更有效地利用处理器的并行性和寄存器操作。不过，这个C语言版本的函数更易理解和维护，
  //适用于较小的数据集或不需要高性能的情况。

//对整数数组 p 中的元素进行按整数 s 进行缩放（乘法）操作。
void scale_asm(int *p, unsigned long long n, int s)
{
  //计算数组的总大小（以字节为单位），其中sizeof(int)是每个整数的大小，n是数组中的元素数量。
  unsigned long long sizeb = sizeof(int) * n;

  //
  __asm__ volatile(
		   "xor %%rcx, %%rcx;\n" //这行汇编代码将寄存器RCX 设置为0，用于循环计数器。
		   
		   "1:;\n"  //这是一个标签，表示循环的开始。
		   
		   "mov (%[_p], %%rcx), %%eax;\n" //这行汇编代码从内存地址 p + RCX 处加载一个整数，将其存储在寄存器EAX 中。
		   "imul %[_s], %%eax;\n"         //这行汇编代码将EAX 寄存器中的值乘以输入参数s，实现整数缩放。
		   "mov %%eax, (%[_p], %%rcx);\n" //这行汇编代码将EAX 寄存器中的结果写回内存地址 p + RCX 处，即将数组中的元素进行缩放后的值写回。
		   
		   "add $4, %%rcx;\n"             //这行汇编代码将RCX 寄存器增加4，以便移动到数组中的下一个整数元素。
		   "cmp %[_sizeb], %%rcx;\n"      //这行汇编代码将RCX 寄存器的值与数组总大小 sizeb 进行比较，以检查是否已经处理完整个数组。   
		   "jl 1b;\n"        //如果RCX 小于 sizeb，则跳转回标签 1 处，继续下一轮迭代。                 
		   
		   : //outputs
		     
		   : //inputs  这是输入操作数部分，指示将数组指针 p、缩放因子 s 和数组总大小 sizeb 传递给汇编代码。
		     [_p]     "r" (p),
		     [_s]     "r" (s),
		     [_sizeb] "r" (sizeb)
		     
		   : //clobber
		     "cc", "memory", "eax", "rcx");
} //该函数用于对数组 p 中的每个元素进行缩放操作，将每个元素乘以整数 s，然后将结果写回数组中，
  //以实现整数数组的缩放操作。这段代码使用内联汇编来优化数组操作，提高了执行效率。

//演示和比较使用C语言和内联汇编语言编写的缩放整数数组的功能
int main(int argc, char **argv)
{
  //程序检查命令行参数的数量，如果少于3个参数（程序名、n、s），则打印使用说明并返回1，表示出现了错误。
  if (argc < 3)
    return printf("Usage: %s [n] [s]\n", argv[0]), 1;

  //程序将命令行参数argv[1]（第二个参数）转换为无符号长整数类型 n，表示数组的大小；
  unsigned long long n = atoi(argv[1]);

  //将argv[2]（第三个参数）转换为整数 s，表示缩放因子。
  int s = atoi(argv[2]);
  
  if (n < 5) //程序检查 n 是否小于5，如果小于5，打印错误消息并返回2，表示 n 必须大于或等于5。
    return printf("Error: 'n' must be >= 5\n"), 2;
  
  //使用 malloc 分配内存来创建一个整数数组 p，大小为 n 个整数。如果内存分配失败，打印错误消息并返回3。
  int *p = malloc(sizeof(int) * n);

  if (!p) //
    return printf("Error: cannot allocate memory\n"), 3; //

  //初始化数组 p，将每个元素设置为递增的整数值。
  for (unsigned long long i = 0; i < n; i++)
    p[i] = i + 1;

  //
  printf("\t== C ==\n");
  
  //Scale the vector by s
  scale_c(p, n, s);

  //使用C语言版本的 scale_c 函数对数组 p 中的元素进行缩放操作，并打印缩放后的数组。
  for (unsigned long long i = 0; i < n; i++)
    printf("%5d%c", p[i], ((i + 1) % 10) ? '\t' : '\n');

  putchar('\n');

  // ==== ASSEMBLY ====
  printf("\t== ASM ==\n");
  
  //重新初始化数组 p，将每个元素再次设置为递增的整数值。
  for (unsigned long long i = 0; i < n; i++)
    p[i] = i + 1;

  //Scale the vector by s
  scale_asm(p, n, s);

  //使用内联汇编版本的 scale_asm 函数对数组 p 中的元素进行缩放操作，并打印缩放后的数组。
  for (unsigned long long i = 0; i < n; i++)
    printf("%5d%c", p[i], ((i + 1) % 10) ? '\t' : '\n');

  putchar('\n');

  //释放动态分配的内存（数组 p）并返回0，表示成功执行。
  free(p);
  
  //
  return 0;
} //这个程序用于演示如何使用C语言和内联汇编语言实现对整数数组的缩放操作，并比较它们的效果。
  //它还包括了一些输入验证，如检查数组大小是否大于或等于5以及内存分配是否成功。程序的输出将包括原始数组和经过缩放操作后的数组。
