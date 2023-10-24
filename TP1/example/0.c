/*
  This code is meant as an example of to write inline x86 assembly code.
  
  Compilation:
      $ gcc -O1 0.c -o 0
      
  Execution:
      $ ./0 10 3

  For more info about inline assembly:

  https://gcc.gnu.org/onlinedocs/gcc/Extended-Asm.html
*/

//
#include <stdio.h>
#include <stdlib.h>

//Example of an addition without register labeling.
//Registers are addressed using their positionning 0, 1, 2, ...
int asm_iadd0(int a, int b)
{
  //
  int s = 0; //在C函数中声明一个整数s，并将其初始化为0。

  __asm__ volatile( //这是内联汇编的开始。__asm__关键字用于告诉编译器后面的内容是汇编指令。
                    //volatile关键字告诉编译器不要优化这段汇编代码，以确保代码按照指定的顺序执行。
		   
		   "add %1, %2;\n" //这是内联汇编中的第一条汇编指令。
                       //它执行加法操作，将%1（代表输入参数a）和%2（代表输入参数b）相加，并将结果存储在%2（输入参数b）中。
                       //这实际上相当于执行了b += a 的操作。
		   
		   "mov %2, %0;\n" //这是内联汇编中的第二条汇编指令。
                       //它执行了一个移动操作，将%2（输入参数b，现在包含相加后的值）的值复制到%0（输出参数s）中，即s = b。
		   
		   : //outputs  这是输出操作数部分，指示将%0（即s）的值返回到C代码中。
		     "=r" (s)  //%0 is the register's ID  

		   : //inputs  这是输入操作数部分，指示将%1（a）和%2（b）作为输入传递给汇编代码。
		     "r" (a),  //%1
		     "r" (b)   //%2  
		     
		   : //clobber 这是指示编译器在内联汇编块中哪些寄存器和内存位置可能会被修改的部分。
         //"cc" 表示可能会影响条件码寄存器，而"memory" 表示可能会影响内存。
		     "cc", "memory");

  return s;  //最后，函数返回s的值，即b 和 a 相加后的结果。
} //这段代码通过内联汇编实现了整数相加操作，并将结果存储在s中，以便在C代码中使用。

//Using register labels
int asm_iadd1(int a, int b)
{
  //
  int s = 0; //在C函数中声明一个整数s，并将其初始化为0。

  __asm__ volatile( //这是内联汇编的开始。__asm__关键字用于告诉编译器后面的内容是汇编指令。
                    //volatile关键字告诉编译器不要优化这段汇编代码，以确保代码按照指定的顺序执行。
		   
		   "add %[_a], %[_b];\n" //这是内联汇编中的第一条汇编指令。
                             //它执行加法操作，将%[_a]（代表输入参数a）和%[_b]（代表输入参数b）相加，并将结果存储在%[_b]（输入参数b）中。
                             //这实际上相当于执行了b += a的操作。

		   "mov %[_b], %[_s];\n" //这是内联汇编中的第二条汇编指令。
                             //它执行了一个移动操作，将%[_b]（输入参数b，现在包含相加后的值）的值复制到%[_s]（输出参数s）中，即s = b。
		   
		   : //outputs  这是输出操作数部分，指示将%[_s]（即s）的值返回到C代码中。[_s]是输出操作数的符号名称，"=r"表示将s视为整数寄存器。
		     [_s] "=r" (s)  //value

		   : //inputs  这是输入操作数部分，指示将%[_a]（a）和%[_b]（b）作为输入传递给汇编代码。
         //[_a] 和 [_b] 是输入操作数的符号名称，"r" 表示将它们视为整数寄存器。
		     [_a] "r" (a),  //value
		     [_b] "r" (b)   //value
		     
		   : //clobber  这是指示编译器在内联汇编块中哪些寄存器和内存位置可能会被修改的部分。
         //"cc" 表示可能会影响条件码寄存器，而"memory" 表示可能会影响内存。
		     "cc", "memory");

  return s; //最后，函数返回s的值，即b 和 a 相加后的结果。
} //这段代码与之前的代码执行相同的任务，但使用了符号名称来提高可读性和维护性。

//
int asm_iadd2(int a, int b)
{
  //
  int s = 0;

  __asm__ volatile(

		   "add %[_a], %[_b];\n" //b += a

		   "mov %[_b], (%[_s]);\n" //*s = b
		   
		   : //outputs
		   : //inputs
		     [_a] "r" (a),  //value
		     [_b] "r" (b),  //value
		     [_s] "r" (&s)  //pointer
		     
		   : //clobber
		     "cc", "memory");

  return s;
}

//
int asm_imul(int a, int b)
{
  //
  int m = 0;

  __asm__ volatile (
		    "imul %[_a], %[_b];\n"

		    "mov %[_b], %[_m];\n"
		    
		    : //outputs
		      [_m] "=r" (m)
		      
		    : //inputs
		      [_a] "r" (a),  //value
		      [_b] "r" (b)   //value

		    : //clobber
		      "cc", "memory");

  return m;
}

//这段代码是一个使用内联汇编的C函数，目的是执行整数除法操作，将两个整数a和b相除，将商存储在指针q指向的位置，余数存储在指针r指向的位置。
void asm_idiv(int a, int b, int *q, int *r)
{
  //Quotient 在C函数中声明两个整数变量qq和rr，分别用于存储商和余数，然后初始化为0。
  int qq = 0;
  
  //Remainder
  int rr = 0;

  __asm__ volatile (  //这两行汇编代码将寄存器EAX（紧接着，RAX的低32位）和寄存器EDX的值都设置为0。
                      //这是为了确保这两个寄存器在开始时都是0，因为它们将在后面的idiv指令中被使用。
		    "xor %%eax, %%eax;\n" //rax = 0
		    "xor %%edx, %%edx;\n" //rdx = 0

		    //Set eax value to a  这行汇编代码将输入参数a的值移动到寄存器EAX中，以便执行idiv指令。
		    "mov %[_a], %%eax;\n"
		    
		    //Divides RAX by operand --> RAX = a / b, RDX = a % b   
        //这行汇编代码执行整数除法，将EAX中的值（被除数，即a）除以%[_b]（输入参数b），商存储在EAX中，余数存储在EDX中。
		    "idiv %[_b];\n"
		    
		    //这两行汇编代码将EAX中的商和EDX中的余数移动到输出参数q和r所指向的内存位置。这实际上将商和余数存储在了指定的变量中。
		    "mov %%eax, %[_q];\n"
		    "mov %%edx, %[_r];\n"
		    
		    : //outputs 这是输出操作数部分，指示将EAX和EDX的值分别返回到C代码中的qq和rr变量中。
		      [_q] "=r" (qq), //value
		      [_r] "=r" (rr)  //value
		      
		    : //inputs  这是输入操作数部分，指示将a和b作为输入传递给汇编代码。
		      [_a] "r" (a),  //value
		      [_b] "r" (b)   //value

		    : //clobber  这是指示编译器在内联汇编块中哪些寄存器和内存位置可能会被修改的部分。
          //"cc" 表示可能会影响条件码寄存器，而"memory" 表示可能会影响内存。
          //同时，还标明了EAX和EDX寄存器会在内联汇编中被修改。
		      "cc", "memory", "eax", "edx");

  //最后，将计算得到的商和余数分别存储在指针q和r指向的内存位置，以便在C代码中使用。
  *q = qq;
  *r = rr;
} //这段代码执行整数除法操作，同时将商和余数存储在指定的变量中。

//
int asm_reduc(int *p, int n)
{
  //在C函数中声明一个整数变量r，并将其初始化为0，以用于累加和。
  int r = 0;

  //计算整数数组的总字节数，其中n是数组中的元素数量，而sizeof(int)是一个整数所占的字节数。这将用于控制循环的迭代次数。
  int s = sizeof(int) * n;

  //
  __asm__ volatile (
		    //这两行汇编代码将寄存器EAX 和 RCX 的值都设置为0。EAX 用于累加和，RCX 用于迭代数组中的元素。
		    "xor %%eax, %%eax;\n" //eax = 0 --> accumulation register
		    "xor %%rcx, %%rcx;\n" //rcx = 0 --> array element index
		    
		    //这是一个标签，用于表示循环的入口点。
		    "1:;\n"
		    
		    //这行汇编代码从内存地址 p + rcx 处加载一个整数值，然后将其与累加寄存器 EAX 相加，实现了累加的功能。
		    "add (%[_p], %%rcx), %%eax;\n"
		    
		   //Loop control
		    "add $4, %%rcx;\n"     //这行汇编代码将 RCX 增加4个字节，即一个整数的大小，以便移动到数组中的下一个元素。
		    "cmp %[_s], %%ecx;\n"  //这行汇编代码比较 ECX（RCX的低32位）与数组的总字节数 s，以确定是否还有数组元素需要累加。如果 ECX 小于 s，说明还有元素需要处理。
		    "jl 1b;\n"             //这是条件跳转指令，如果 ECX 小于 s，则跳转到标签 1，即回到循环的开始，继续累加下一个数组元素。
		    
		    "mov %%eax, %[_r];\n"  //这行汇编代码将累加的结果从 EAX 寄存器移动到输出参数 r 所指向的内存位置，以便在C代码中使用。
		    
		    : //outputs 这是输出操作数部分，指示将 EAX 寄存器的值返回到C代码中的变量 r 中。
		      [_r] "=r" (r)
		      
		    : //inputs  这是输入操作数部分，分别指示将整数数组的地址 p 和数组总字节数 s 传递给汇编代码。
		      [_p] "r" (p),
		      [_s] "r" (s)
		      
		    : //clobber  这是指示编译器在内联汇编块中哪些寄存器和内存位置可能会被修改的部分。
          //"cc" 表示可能会影响条件码寄存器，而 "memory" 表示可能会影响内存。
          //同时，还标明了 EAX 和 RCX 寄存器会在内联汇编中被修改。
		      "cc", "memory", "eax", "rcx"
		    );
  
  //最后，函数返回累加和 r 的值，即整数数组中所有元素的总和。
  return r;
} //这段代码使用内联汇编实现了累加操作，遍历整数数组并将元素的值相加，最终返回累加和。

int C_reduc(int *p, int n)
{
  int r = 0;

  for (int i = 0; i < n; i++)
    r += p[i];

  return r;
}

//
int main(int argc, char **argv)
{
  //
  if (argc < 3)
    return printf("Usage: %s a b\n", argv[0]), 1;

  //
  int a = atoi(argv[1]);
  int b = atoi(argv[2]);

  int q = 0;
  int r = 0;
  
  //
  printf("%d + %d = %d\n"  , a, b, asm_iadd0(a, b));
  printf("%d + %d = %d\n"  , a, b, asm_iadd1(a, b));
  printf("%d + %d = %d\n\n", a, b, asm_iadd2(a, b));

  //
  printf("%d * %d = %d\n\n", a, b, asm_imul(a, b));

  //
  asm_idiv(a, b, &q, &r);
  
  printf("%d / %d => q: %d, r: %d\n\n", a, b, q, r);

  //
  int p[100];
  int n = 100;

  //Initialize array 
  for (int i = 0; i < n; i++)
    p[i] = i + 1;

  //
  int r_asm = asm_reduc(p, 100);
  int r_C   = C_reduc(p, n);

  //
  printf("reduc asm: %d\n", r_asm);
  printf("reduc C  : %d\n", r_C);
  
  //
  return 0;
}
