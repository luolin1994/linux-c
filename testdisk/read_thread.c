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
#include <pthread.h>
#define NUM_Threads 5
/* asynchronous I/O control block */
/*
*结构体参数传递的线程并发编程，读取设备文件
* pthread库不是Linux系统默认的库，连接时需要使用库libpthread.a,
* 在使用pthread_create创建线程时，在编译中要加-lpthread参数:
* gcc createThread.c -lpthread -o createThread
* ./createThread.o
* 加上上面两句以后编译成功，并输出结果
*/

/*
aio：只支持使用O DIRECT标志打开的文件
基于线程的异步I/O

*/

typedef struct thread_data
{
    int fd;
    int blocksize;
    int offset; 
    int threadid;
}THDATA, *PTHDATA;

void  * preaddisk(void * pThreadData){
    char *buf= (char *)malloc(1024* sizeof(char) ); 
    PTHDATA tData= (PTHDATA)pThreadData;
    printf("Pthread : %d 开始执行 \n",tData->threadid);
    ssize_t ret;
    if((ret = pread(tData->fd,buf,tData->blocksize,tData->offset)) == -1){
        printf("pread %d failed\n",tData->threadid);
    }else{
        printf("线程 %d pread 操作成功!\n",tData->threadid);
        printf("线程 %d read buf is:  %s\n",tData->threadid,buf);
    }
    free(buf);
    
}


void main(int argc, char *argv[]){

    //获取程序开始执行时间
    struct timeval start, end;
    float time_use=0;
    gettimeofday(&start,NULL);
    
    //打开设备文件
    const char* pathname="/dev/sdb";
    int fd=open(pathname,O_RDONLY);
    if (fd==-1) {
        printf("%s",strerror(errno));
    }

    
    pthread_t Pthread[NUM_Threads];
    THDATA index[NUM_Threads];
    int i,ret;
    for ( i = 0; i <NUM_Threads; i++)
    {
        printf("main() : 创建线程 %d \n",i);
        index[i].threadid = i;
        index[i].fd = fd;
        index[i].blocksize = 20;
        index[i].offset = 128 + 1024*i;
        ret = pthread_create(&Pthread[i],NULL, preaddisk,(void *)&index[i]);
        if (0 != ret){
            printf("Error: 创建线程失败！\n");
        }
    }

    //使用pthread_join等待子线程的结束，运行prhtead_join之后主线程就阻塞了
    int j;
    for (j = 0; j < NUM_Threads; j++){
        int rett = pthread_join(Pthread[j],NULL);
        if(0 != rett){
            printf("Error: unable to join,%d\n",rett);
        }
    }
    
    //pthread_exit(NULL);
    close(fd);

    //获取程序结束时间并计算程序执行时间
    gettimeofday(&end,NULL);
    time_use=(end.tv_sec-start.tv_sec)*1000000+(end.tv_usec-start.tv_usec);//微秒
    printf("time_use is %.10f us\n",time_use);
    //printf("when the blocksize is %d MB, the read speed is %.10f MB/s\n");
    
    printf("the main() thread end!\n");
    

}

/**
 *linux下主线程return 0和pthread_exit(NULL)的区别
 * 1. 当linux和Windows中,主线程以return 0结束时,程序会在主线程运行完毕后结束，即整个进程结束.
 * 2. 当linux中,主线程以pthread_exit(NULL)作为返回值,则主线程会等待子线程.
 * 在main线程中调用pthread_exit会起到只让main线程退出，但是保留进程资源，
 * 供其他由main创建的线程使用，直至所有线程都结束，但在其他线程中不会有这种效果
 * */