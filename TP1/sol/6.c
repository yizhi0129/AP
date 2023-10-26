//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

//
typedef unsigned char      u8;
typedef unsigned long long u64;

//这是一个结构体定义，表示 PPM (Portable Pixmap) 格式图像的信息。PPM 是一种无损的位图图像格式，通常用于存储彩色或灰度图像。
typedef struct {

  //分别表示图像的宽度和高度，以无符号64位整数 (u64) 存储。这些值确定了图像的像素尺寸。
  u64 w;
  u64 h;

  //表示图像的颜色深度或类型。通常，PPM 图像的颜色深度为 8 位（灰度图像）或 24 位（彩色图像）。
  //这里使用 u64 存储，可能用于表示其他颜色深度。
  //transparence included (32 not enough)
  u64 t;
  
  //一个指向图像像素数据的指针。像素数据通常以一维数组的形式存储，每个像素包含一个或多个颜色通道的值。
  //u8 表示无符号8位整数，通常用于表示像素的颜色通道值。
  u8 *pixels;

} ppm_t;

//Loads a PPM file with pixels stored in binary format P6
//Netpbm format details can be found here: https://en.wikipedia.org/wiki/Netpbm#Description
//加载 PPM 图像文件并创建一个 ppm_t 结构
ppm_t *load_ppm(const char *fname)
{
  //检查文件名是否为 NULL，如果为 NULL，将打印错误消息并返回 NULL
  if (!fname)
    return printf("Error: file name is NULL"), NULL;

  //
  u64 w = 0;
  u64 h = 0;
  u64 t = 0;
  
  //打开指定文件，如果文件无法打开，也将打印错误消息并返回 NULL
  FILE *fp = fopen(fname, "rb");

  if (!fp)
    return printf("Error: cannot open file '%s'\n", fname), NULL;

  //Create PPM holder
  //创建一个 ppm_t 结构体指针 p 并分配内存以存储图像数据，如果内存分配失败，同样会打印错误消息并返回 NULL
  ppm_t *p = malloc(sizeof(ppm_t));
  
  if (!p)
    return printf("Error: cannot allocate memory for ppm file\n"), NULL;

  //读取 PPM 图像文件的标识符，通常为 "P6"，如果标识符不匹配，将打印错误消息并返回 NULL。
  u8 id1 = 0, id2 = 0;
  
  fscanf(fp, "%c%c\n", &id1, &id2);

  if (id1 != 'P' || id2 != '6')
    return printf("Error: only PPM P6 binary format is handled\n"), NULL;
  
  //Read width and height of the image
  //读取图像的宽度和高度，并将它们存储在 w 和 h 变量中。
  fscanf(fp, "%llu %llu\n", &w, &h);

  //Reading threshold
  //读取颜色深度或阈值，并将其存储在 t 变量中。
  fscanf(fp, "%llu\n", &t);
  
  //将图像的宽度、高度和颜色深度存储在 ppm_t 结构体的相应成员中。
  p->w = w;
  p->h = h;
  p->t = t;
  
  //Pixels are stored in RGB (3 bytes), hence the w * h * 3.
  //分配足够的内存来存储图像的像素数据，每个像素由 RGB 三个通道组成，因此需要分配 w * h * 3 字节的内存。
  p->pixels = malloc(sizeof(u8) * w * h * 3);

  if (!p->pixels)
    return printf("Error: cannot allocate memory for pixels\n"), NULL;

  //使用 fread 函数从文件中读取像素数据，并将其存储在 p->pixels 中。
  size_t read_bytes = fread(p->pixels, sizeof(u8), w * h * 3, fp);

  //关闭文件
  fclose(fp);
  
  //检查读取的字节数是否与图像分辨率相匹配，如果不匹配，将打印错误消息并返回 NULL
  if (read_bytes != (w * h * 3))
    return printf("Error: mismatch between read bytes and image resolution\n"), NULL;
  
  //返回指向 ppm_t 结构的指针，该结构包含了图像的信息和像素数据
  return p;
} //这个函数用于加载 PPM 图像文件，将图像信息存储在 ppm_t 结构中，并返回指向该结构的指针。
  //如果加载失败，它会返回 NULL 并打印相关错误消息。

//
ppm_t *create_ppm(u64 w, u64 h, u64 t)
{
  //分配内存以容纳ppm_t结构，并检查内存分配是否成功。
  ppm_t *p = malloc(sizeof(ppm_t));

  if (!p)
    return printf("Error: cannot allocate memory for ppm\n"), NULL;
  
  //设置图像的宽度、高度和色深。
  p->w = w;
  p->h = h;
  p->t = t;
  
  //分配内存以容纳像素数据，并检查内存分配是否成功。
  p->pixels = malloc(sizeof(u8) * w * h * 3);

  if (!p->pixels)
    return printf("Error: cannot allocate memory for pixels\n"), NULL;

  //First touch through initialization
  //使用memset函数将像素数据初始化为零
  memset(p->pixels, 0, w * h *3);
  
  //
  return p;
} //这个函数的目的是创建一个空白的PPM图像，可以在后续的图像处理中填充像素数据。
  //在填充像素数据后，不要忘记使用release_ppm函数来释放内存，以防止内存泄漏。

//这是一个用于将PPM格式图像写入文件的函数。它接受两个参数，一个是指向ppm_t结构的指针（表示要写入的图像），
//另一个是要写入的文件的文件名。该函数将图像数据以二进制格式写入指定的文件中。
void write_ppm(ppm_t *p, const char *fname)
{
  //检查传递给函数的指针是否为NULL，以确保图像和文件名都有效。
  if (!p || !fname)
    printf("Error: pointer is NULL"), exit(-1);

  //打开指定文件以供写入，如果文件无法打开，则报告错误并退出。
  FILE *fp = fopen(fname, "wb");

  if (!fp)
    printf("Error: cannot create file '%s'\n", fname), exit(-1);

  //Writing format identifier
  //写入PPM文件的格式标识符（通常为 "P6"，表示二进制PPM格式）。
  fprintf(fp, "P6\n");
  
  //Writing image dimensions
  //写入图像的宽度和高度。
  fprintf(fp, "%llu %llu\n", p->w, p->h);

  //写入色深信息。
  fprintf(fp, "%llu\n", p->t);
  
  //Writing pixels in binary format
  //以二进制格式写入图像的像素数据。
  fwrite(p->pixels, sizeof(u8), p->w * p->h * 3, fp);

  //关闭文件。
  fclose(fp);
} //此函数用于将图像数据保存到PPM格式文件中，以便后续可以加载和显示。
 
//release_ppm 函数用于释放 ppm_t 结构和其包含的像素数据内存。函数的主要任务是确保释放所有分配的内存，以免发生内存泄漏。
void release_ppm(ppm_t *p)
{  //函数检查传递给它的 ppm_t 结构指针 p 是否为 NULL，以确保它不会试图释放空指针。
  if (p) 
    { //如果 p 不为空，函数继续执行。它进一步检查 p 中的 pixels 成员是否为空。
      //如果 pixels 不为空，它释放了该内存，以释放图像像素数据。
      if (p->pixels) 
	free(p->pixels);

      p->w = 0; //函数将 w 和 h 成员都设置为0，以指示 ppm_t 结构不再引用有效的图像数据。
      p->h = 0;
    }
  else
    { //如果 p 为空（即传递给函数的指针为空），函数将报告错误并终止程序。
      printf("Error: pointer is NULL\n");
      exit(-1); 
    } //这个函数的主要目的是确保在不再需要 ppm_t 结构时释放相关的内存，以免内存泄漏。
}

//Inverts the pixels
//这是一个C语言函数，用于将输入数组 in 中的每个元素逐个反转，并将结果存储在输出数组 out 中。
//反转的方式是将每个元素减去255，这实际上是对8位无符号整数进行求补操作。该函数用于颠倒颜色值或将图像进行反色处理。
void invert_c(u8 *in, u64 len, u8 *out)
{
  //u8 *in: 输入数组，包含待反转的元素。
  //u64 len: 输入数组的长度，即要处理的元素个数。
  //u8 *out: 输出数组，用于存储反转后的结果。
  for (u64 i = 0; i < len; i++)
    out[i] = 255 - in[i];
} //函数遍历输入数组中的每个元素，将每个元素的值减去255，然后将结果存储在输出数组中。
  //这将导致颜色值从255减到0，从0减到255，实现了颜色的反转。


void invert_asm(u8 *in, u64 len, u8 *out)
{
  //这是一个使用内联汇编的C函数，用于将输入数组 in 中的每个8字节的元素逐个反转，并将结果存储在输出数组 out 中。
  //这是一种高效的反转操作，将输入的8字节值的每个位都进行翻转（0变为1，1变为0）。
  __asm__ volatile(
		   "xor %%rcx, %%rcx;\n" //该函数使用了x86_64体系结构的汇编指令。在循环中，它依次处理输入数组中的每个8字节元素（64位元素）。

		   "1:;\n"

		   "movq (%[_in], %%rcx), %%rax;\n"   //使用movq指令将输入数组中的8字节元素加载到寄存器rax中。
		   "not %%rax;\n"                     //使用not指令对rax中的值进行按位取反操作，实现了反转。
		   "movq %%rax, (%[_out], %%rcx);\n"  //使用movq指令将结果存储回输出数组中。
		   
		   "add $8, %%rcx;\n"      //增加一个偏移，以处理下一个8字节元素，直到处理完整个数组。
		   "cmp %[_s], %%rcx;\n"
		   "jl 1b;\n"
		   
		   : //outputs

		   : //inputs
		     [_in]  "r" (in),
		     [_out] "r" (out),
		     [_s]   "r" (len)
		     
		   : //clobber
		     "cc", "memory", "rcx"
		   );  //这个函数实现了高效的位级别反转，通常用于需要快速处理大型数据的应用程序
}

//这段C程序的目的是从输入的PPM图像文件中加载图像，然后使用不同的方式反转像素，并将反转后的图像保存到两个不同的输出文件中。
int main(int argc, char **argv)
{
  //检查命令行参数，确保提供了输入PPM图像文件的文件名。
  if (argc < 2)
    return printf("Usage: %s [ppm input image]\n", argv[0]), 1;

  //使用load_ppm函数加载输入的PPM图像文件并存储在ppm_t结构体 p_in 中。
  ppm_t *p_in = load_ppm(argv[1]);
  
  if (!p_in)
    exit(-1);

  //使用create_ppm函数创建两个ppm_t结构体 p_out_c 和 p_out_asm，用于分别存储C版本和汇编版本的反转结果。
  ppm_t *p_out_c = create_ppm(p_in->w, p_in->h, p_in->t);

  if (!p_out_c)
    exit(-1);

  
  ppm_t *p_out_asm = create_ppm(p_in->w, p_in->h, p_in->t);

  if (!p_out_asm)
    exit(-1);

  
  //使用invert_c函数将输入图像的像素进行反转，结果存储在 p_out_c 中。
  invert_c(p_in->pixels, p_in->w * p_in->h * 3, p_out_c->pixels);

  //使用invert_asm函数将输入图像的像素进行汇编方式的反转，结果存储在 p_out_asm 中。
  invert_asm(p_in->pixels, p_in->w * p_in->h * 3, p_out_asm->pixels);

  //使用write_ppm函数将 p_out_c 和 p_out_asm 中的反转图像分别保存到 "out_c.ppm" 和 "out_asm.ppm" 文件中。
  write_ppm(p_out_c, "out_c.ppm");
  write_ppm(p_out_asm, "out_asm.ppm");

  //释放内存并清理所有分配的资源。
  release_ppm(p_in); free(p_in);
  release_ppm(p_out_c); free(p_out_c);
  release_ppm(p_out_asm); free(p_out_asm);
  
  //
  return 0;
}
