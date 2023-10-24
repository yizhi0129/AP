//
#include <stdio.h>
#include <stdlib.h>

//
unsigned long long fibo_c(unsigned long long n)
{
  //
  if ((n == 0) || (n == 1))
    return 1;
  
  //
  unsigned long long t = 0;
  unsigned long long f0 = 1;
  unsigned long long f1 = 1;

  //
  for (unsigned long long i = 1; i < n; i++)
    {
      t = f1;
      f1 += f0;
      f0 = t;
    }

  //
  return f1;
}

//计算斐波那契数列中的第n项数值
unsigned long long fibo_asm(unsigned long long n)
{
  //这是一个条件判断，如果n等于0或1，直接返回1。因为斐波那契数列中的前两项都是1，所以这是一个边界条件。
  if ((n == 0) || (n == 1))
    return 1;

  //在C函数中声明一个无符号长整数变量f，并将其初始化为1，用于存储计算结果。
  unsigned long long f = 1;

  //.intel_syntax noprefix和.att_syntax prefix之间的部分指示了在此汇编块中使用的语法。
  __asm__ volatile (
		    ".intel_syntax noprefix;\n"
		    
		    "mov rcx, 1;\n" //这行汇编代码将寄存器RCX 设置为1，用于迭代计算斐波那契数列。
		    
		    "mov rax, 1;\n" //这两行汇编代码将寄存器RAX 和 RBX 分别设置为1，用于存储斐波那契数列中的两个相邻项。
		    "mov rbx, 1;\n" //

		    "loop:;\n" //这是一个标签，表示循环的开始。
		    
		    "mov rdx, rax;\n" //这行汇编代码将寄存器RDX 设置为RAX的值，实现了一个临时变量t来保存上一次f1的值。
		    "add rax, rbx;\n" //这行汇编代码将RAX寄存器（f1）与RBX寄存器（f0）相加，实现了斐波那契数列的递推关系：f1 = f1 + f0。
		    "mov rbx, rdx;\n" //这行汇编代码将RBX寄存器（f0）设置为t，即上一次的f1值。
		    
		    "add rcx, 1;\n"   //这行汇编代码将RCX寄存器（迭代计数器 i）增加1，表示迭代次数加1。
		    "cmp rcx, %1;\n"  //这行汇编代码将RCX寄存器与输入参数n进行比较，检查是否已经计算到第n项。
		    "jl loop;\n"      //如果RCX小于n，则跳转回标签loop，继续下一轮迭代。

		    "mov %0, rax;\n" //这行汇编代码将RAX寄存器中的值（最终的斐波那契数列第n项的值）存储到C代码中的输出参数f中。
		    
		    ".att_syntax prefix;\n"  //这部分告诉编译器要回到使用AT&T汇编语法。

		    : //outputs 这是输出操作数部分，指示将f的值返回到C代码中的变量 f 中，同时告诉编译器它是一个只写输出操作数（"=&r"）。
		      "=r" (f)
		      
		    : //inputs  这是输入操作数部分，指示将输入参数 n 传递给汇编代码。
		      "r" (n)
		    
		    :
		    "cc", "memory", "rax", "rbx", "rcx", "rdx");
  
  //函数返回斐波那契数列中第n项的值，该值存储在变量 f 中
  return f;
}

//用于计算并比较斐波那契数列中的第n项的值，同时测试C语言和内联汇编版本的斐波那契数列计算函数。
int main(int argc, char **argv)
{
  //程序检查命令行参数的数量，如果少于2个参数（只有程序名和n），则打印使用说明并返回1，表示出现了错误。
  if (argc < 2)
    return printf("Usage: %s [n]\n", argv[0]), 1;

  //程序将命令行参数argv[1]（第二个参数）转换为无符号长整数类型，并存储在变量n中。
  unsigned long long n = atoll(argv[1]);

  //程序使用fibo_c和fibo_asm函数分别计算斐波那契数列中的第n项，并将它们的结果打印出来。
  printf("fibo_c(%llu): %llu\n", n, fibo_c(n));
  printf("fibo_asm(%llu): %llu\n", n, fibo_asm(n));
  
  //程序返回0，表示成功执行。
  return 0;
} //这个程序的主要功能是接受一个命令行参数n，然后使用C语言和内联汇编两种方法来计算斐波那契数列中第n项的值，并将结果打印出来以供比较。
