//
#include <stdio.h>
#include <stdlib.h>

//计算两个整数数组 a 和 b 的点积（dot product）
//整数指针 a，表示第一个整数数组；整数指针 b，表示第二个整数数组；无符号长整数 n，表示数组的大小。
long long dotprod_c(int *a, int *b, unsigned long long n) 
{
  //在函数内部，创建一个长整数变量 d，用于存储点积的累积值，初始值为0。
  long long d = 0;

  //使用 for 循环遍历两个数组中的每个元素，循环变量 i 从0递增到 n-1。
  for (unsigned long long i = 0; i < n; i++)
    d += (a[i] * b[i]); //在循环中，计算 a[i] 与 b[i] 的乘积，并将结果累积到变量 d 中，实现了点积的计算。

  //循环结束后，函数返回点积的结果，即变量 d 中的值。
  return d;
} //这个函数非常简单且易于理解，它执行了一个常见的数学运算——计算两个数组的点积。
  //点积通常用于向量计算、线性代数等领域，可以用来度量两个向量之间的相似性或关联性。
  //这个C语言版本的函数可以适用于小到中等大小的数组，但在处理大型数据集时可能需要优化以提高性能。

//采用了内联汇编以优化点积的计算
long long dotprod_asm(int *a, int *b, unsigned long long n)
{
  //创建一个长整数变量 d，用于存储点积的累积值，初始值为0。
  long long d = 0;

  //计算数组的总大小（以字节为单位），其中sizeof(int)是每个整数的大小，n是数组中的元素数量。
  unsigned long long sizeb = sizeof(int) * n;
  
  //
  __asm__ volatile(
		   "xor %%rcx, %%rcx;\n"  //这两行汇编代码分别将寄存器 RCX 和 RDX 设置为0，用于循环计数器和累积点积结果。
		   "xor %%rdx, %%rdx;\n"
		   
		   "1:;\n"  //这是一个标签，表示循循环的开始。
		   "mov (%[_a], %%rcx), %%eax;\n" //这两行汇编代码从内存地址 a + RCX 和 b + RCX 处加载整数，并将它们分别存储在寄存器 EAX 和 EBX 中。
		   "mov (%[_b], %%rcx), %%ebx;\n" 

		   "imul %%ebx, %%eax;\n"       //这行汇编代码将 EAX 寄存器中的值与 EBX 寄存器中的值相乘，实现了两个数组元素的乘法操作。
		   "add %%eax, %%edx;\n"        //这行汇编代码将 EAX 寄存器中的结果加到 EDX 寄存器中，用于累积点积的总和。
		   
		   "add $4, %%rcx;\n"         //这行汇编代码将 RCX 寄存器增加4，以便移动到数组中的下一个整数元素。
		   "cmp %[_sizeb], %%rcx;\n"  //这行汇编代码将 RCX 寄存器的值与数组总大小 sizeb 进行比较，以检查是否已经处理完整个数组。
		   "jl 1b;\n"                 //如果 RCX 小于 sizeb，则跳转回标签 1 处，继续下一轮迭代。

		   "mov %%rdx, %[_d];\n"      //将累积点积结果 EDX 寄存器中的值写回变量 d 中，以便作为函数返回值。
		   
		   : //outputs   这是输出操作数部分，指示将点积结果 d 传递给汇编代码，并将其写回 C 变量。
		     [_d] "=r" (d)
		     
		   : //inputs    这是输入操作数部分，指示将数组指针 a 和 b 以及数组总大小 sizeb 传递给汇编代码。
		     [_a]     "r" (a),
		     [_b]     "r" (b),
		     [_sizeb] "r" (sizeb)
		     
		   : //clobber
		     "cc", "memory", "eax", "ebx", "rcx", "rdx"
		   );

  //函数返回累积点积的结果，即变量 d 中的值。
  return d;
} //这个函数使用内联汇编优化了点积的计算，可以提高对大型数据集的计算性能。

//演示和比较使用C语言和内联汇编语言编写的点积（dot product）计算功能
int main(int argc, char **argv)
{
  //程序检查命令行参数的数量，如果少于2个参数（程序名和n），则打印使用说明并返回1，表示出现了错误。
  if (argc < 2)
    return printf("Usage: %s [n]\n", argv[0]), 1;

  //程序将命令行参数argv[1]（第二个参数）转换为无符号长整数类型 n，表示要计算点积的数组的大小。
  unsigned long long n = atoll(argv[1]);
  
  //程序检查 n 是否小于5，如果小于5，打印错误消息并返回2，表示 n 必须大于或等于5。
  if (n < 5) 
    return printf("Error: 'n' must be >= 5\n"), 2;
  
  //使用 malloc 分配内存来创建两个整数数组 a 和 b，每个数组的大小为 n 个整数。如果内存分配失败，打印错误消息并返回3。
  int *a = malloc(sizeof(int) * n);
  int *b = malloc(sizeof(int) * n);

  if (!a || !b)
    return printf("Error: cannot allocate memory\n"), 3;
  
  //初始化数组 a 和 b，分别将它们的元素设置为 a[i] = i + 1 和 b[i] = i - 1。
  for (unsigned long long i = 0; i < n; i++)
    {
      a[i] = i + 1;
      b[i] = i - 1;
    }
  
  //使用C语言版本的 dotprod_c 函数计算数组 a 和 b 的点积，并将结果打印出来。
  long long d_c = dotprod_c(a, b, n);

  //Print vector
  printf("dotprod_c: %lld\n", d_c);

    //重新初始化数组 a 和 b，再次将它们的元素设置为 a[i] = i + 1 和 b[i] = i - 1。
  for (unsigned long long i = 0; i < n; i++)
    {
      a[i] = i + 1;
      b[i] = i - 1;
    }
  
  //使用内联汇编版本的 dotprod_asm 函数计算数组 a 和 b 的点积，并将结果打印出来。
  long long d_asm = dotprod_asm(a, b, n);

  //Print vector
  printf("dotprod_asm: %lld\n", d_asm);

  //释放动态分配的内存（数组 a 和 b）并返回0，表示成功执行。
  free(a);
  free(b);
  
  //
  return 0;
} //这个程序用于演示如何使用C语言和内联汇编语言实现点积的计算功能，并比较它们的效果。
  //使用内联汇编的版本可能在大型数据集上具有更高的性能，因为它可以更有效地利用处理器的并行性和寄存器操作。
