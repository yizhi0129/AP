//
#include <stdio.h>
#include <stdlib.h>

//
unsigned long long collatz_c(unsigned long long n)
{
  //Number of iterations
  unsigned long long i = 1;
  unsigned long long v = n;
  
  //
  while (v != 1)
    {
      //Even 
      if ((v % 2) == 0)
	v /= 2;
      else //Odd
	v = (3 * v) + 1;

      //Count iterations
      i++;
    }

  //
  return i;
}

//
unsigned long long collatz_asm(unsigned long long n) {

  //在C函数中声明一个无符号长整数变量i，用于存储迭代次数，初始值为0。
  unsigned long long i = 0;

  __asm__ volatile (

		    "xor %[_i], %[_i];\n" //这行汇编代码将输出操作数i（迭代次数）的值设置为0。
		    "xor %%rax, %%rax;\n" // rax = 0 这两行汇编代码将寄存器RAX 和 RDX 的值都设置为0，它们将在后面的整数除法操作中被使用。
		    "xor %%rdx, %%rdx;\n" // rdx = 0
		    "mov $2, %%ecx\n"     // 这行汇编代码将寄存器ECX 设置为2，作为除数。

		    "loop:\n"  //这是一个标签，表示循环的开始。
		    "inc %[_i];\n"  //这行汇编代码增加迭代次数，相当于 i++。
		    "cmp  $1, %[_v];\n"  //这行汇编代码将输入参数n（%[_v]）与1进行比较，检查是否等于1。
		    "je loop_end;\n"   //如果比较结果为相等（即n 等于1），则跳转到标签 loop_end，结束循环。

		    "test $1, %[_v]\n"  //这两行汇编代码测试输入参数 n 的最低位是否为1，如果是1（奇数），则跳转到标签 odd 处处理奇数情况。
		    "jnz odd;\n"
		    // v is even
		    "mov %[_v], %%rax;\n"  //这行汇编代码将输入参数 n 的值移动到寄存器 RAX 中，以便执行整数除法。
		    "div %%rcx;\n"  //这行汇编代码执行整数除法，将 RAX 寄存器中的值除以 RCX 寄存器中的值，商存储在 RAX 中，余数存储在 RDX 中。
		    "movsx %%eax, %[_v];\n"  //这行汇编代码将 EAX 寄存器中的商（已被扩展为64位有符号整数）移动回输入参数 n 中，更新 n 的值。
		    "jmp loop;\n"  //这行汇编代码跳转回标签 loop，继续下一轮迭代。
		    
		    "odd:\n"  //这是一个标签，表示处理奇数情况。
		    "imul $3, %[_v];\n"  //这行汇编代码将输入参数 n 乘以3，实现Collatz猜想的奇数情况处理。
		    "inc %[_v];\n"  //这行汇编代码将 n 增加1，继续迭代。
		    "jmp loop ;\n"  //这行汇编代码跳转回标签 loop，继续下一轮迭代。
		    "loop_end:\n"   //这是一个标签，表示循环结束。

		    : // outputs  这是输出操作数部分，指示将i 的值返回到C代码中的变量 i 中，同时告诉编译器它是一个只写输出操作数（"=&r"）。
		      [_i] "=&r"(i)
		      
		    : // inputs  这是输入操作数部分，指示将输入参数 n 传递给汇编代码。
		      [_v] "r"(n)
		      
		    : // clobber
		      "cc", "memory", "eax", "rax", "rcx", "rdx");

  return i;  //函数返回迭代次数 i 的值，即Collatz猜想中将输入整数 n 变为1所需的迭代次数
}

//用于计算并比较Collatz猜想的迭代计算结果，同时测试C语言和内联汇编版本的Collatz计算函数。
int main(int argc, char **argv)
{
  //程序检查命令行参数的数量，如果少于2个参数（只有程序名和n），则打印使用说明并返回1，表示出现了错误。
  if (argc < 2)
    return printf("Usage: %s [n]\n", argv[0]), 1;
  
  //程序将命令行参数argv[1]（第二个参数）转换为无符号长整数类型，并存储在变量n中。
  unsigned long long n = atoll(argv[1]);

  //程序检查n是否等于0，如果等于0，打印错误消息并返回2，表示n=0是不允许的情况。
  if (n == 0)
    return printf("n = 0 not allowed!\n"), 2;
  
  //程序使用collatz_c和collatz_asm函数分别计算Collatz猜想中对输入整数n的迭代计算结果，并将它们的结果打印出来。
  printf("collatz_c(%llu)  : %llu\n", n, collatz_c(n));
  printf("collatz_asm(%llu): %llu\n", n, collatz_asm(n));
  
  
  //程序返回0，表示成功执行。
  return 0;
} //这个程序的主要功能是接受一个命令行参数n，然后使用C语言和内联汇编两种方法来计算Collatz猜想中的迭代计算结果，
  //并将结果打印出来以供比较。程序还包括了一些输入验证，例如不允许n等于0。
