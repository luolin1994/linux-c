#include <stdint.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/fs.h>
#include <errno.h> 
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <math.h>

//获取设备块的大小
/*
使用 ioctl 函数的 BLKGETSIZE/BLKGETSIZE64 来获取。
BLKGETSIZE 返回的是有多少块（每块是512字节）
*/

//printf函数需要<stdlib.h>头文件，否则报错隐式声明与内建函数‘printf’不兼容

/*
获取时间时，函数clock()是发现clock()是程序从启动到函数调用占用CPU的时间。
这个函数返回从“开启这个程序进程”到“程序中调用clock()函数”时之间的CPU时钟计时单元（clock tick）数。
*/


int  main(){


    struct timeval start, end;
    float time_use=0;

    gettimeofday(&start,NULL);
    const char* pathname="/dev/sda";
    
    int fd=open(pathname,O_RDONLY);
    if (fd==-1) {
        printf("%s",strerror(errno));
    }
    unsigned long numblocks=0;
    if (ioctl(fd,BLKGETSIZE,&numblocks)==-1) {
         printf("%s",strerror(errno));
    }
    close(fd);
    printf("Number of blocks: %lu, this makes %.3f GB\n", numblocks, 
        (double)numblocks * 512.0 / (1024 * 1024 * 1024));
    
    int i = 5;
    while (i--){
        sleep(1);
    }
    gettimeofday(&end,NULL);

    time_use=(end.tv_sec-start.tv_sec)*1000000+(end.tv_usec-start.tv_usec);//微秒
    printf("time_use is %.10f us\n",time_use);

    return 0;
}