#define _XOPEN_SOURCE 500
#include <stdint.h>
#include <fcntl.h>
#include <linux/fs.h>
#include <errno.h> 
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <math.h>
#include <aio.h>

/**
 * 异步I/O的基本概念：
 * 即我们在调用I/O操作时(读或写)我们的程序不会阻塞在当前位置，而是在继续往下执行。
 * 例如当我们调用异步读API aio_read()时，程序执行此代码之后会接着运行此函数下面的代码，
 * 并且与此同时程序也在进行刚才所要读的文件的读取工作，但是具体什么时候读完是不确定的
 * 
 * 
 * */

void main(){

}
