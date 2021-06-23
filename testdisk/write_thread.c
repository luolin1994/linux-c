#define _XOPEN_SOURCE 500
#include <stdint.h>
#include <fcntl.h>
#include <linux/fs.h>
#include <errno.h> 
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#define NUM_Threads 5

/*
*结构体参数传递的线程并发编程，写入设备文件
* pthread库不是Linux系统默认的库，连接时需要使用库libpthread.a,
* 在使用pthread_create创建线程时，在编译中要加-lpthread参数:
* gcc createThread.c -lpthread -o createThread
* ./createThread.o
* 加上上面两句以后编译成功，并输出结果
*/

typedef struct thread_data
{
    int fd;
    int blocksize;
    int offset; 
    int threadid;
    char * buf_string;
}THDATA, *PTHDATA;


//随机生成字符串，有长度限制，在栈内存上，生成num bytes 大小的随机字符串
void getRandString(char s[],int num){
    int i;
    char *str = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz,./;\"'<>?";
    char ss[2] = {0};
    int lstr = strlen(str);//计算字符串长度
    srand((unsigned int)time((time_t *)NULL));//使用系统时间来初始化随机数发生器
    for( i = 1; i < num; i++){
        //rand()%lstr 可随机返回0-71之间的整数, str[0-71]可随机得到其中的字符
        //函数sprintf(): 字符串格式化命令, 把格式化的数据写入某个字符串中
        sprintf(ss,"%c",str[(rand()%lstr)]);
        strcat(s,ss);
    }
}


//该函数生成 blocksize MB大小的字符串
char * getBufString(int blocksize){

    /*使用 malloc 分配的内存是在堆上而不是在栈内存上面。适用于大块内存。
    但是要记得将其在调用方使用 free 释放申请的内存空间，否则容易造成内存泄漏问题。
    */
   //随机生成1MB blocksize的缓冲区数据
    char rand_string[1024*1024] = {0};
    getRandString(rand_string,1024*1024);
    // printf("the rand string  is : %s\n",s);
    //printf("the length of rand_string  is : %d\n",strlen(rand_string));
    char *buff_string = (char *)malloc(blocksize * 1024 *1024* sizeof(char)); 
    int i;

    for(i =1; i<=blocksize ; i++){
        strcat(buff_string,rand_string);
    }
    //printf("the rand string  is : %s\n",p);
    //printf("the length of buff_string is : %d\n",strlen(buff_string));

    return buff_string;
}


void  * pwritedisk(void * pThreadData){
    PTHDATA tData= (PTHDATA)pThreadData;
    printf("Pthread : %d 开始执行 \n",tData->threadid);
    ssize_t ret;
    if((ret = pwrite(tData->fd,tData->buf_string,tData->blocksize*1024*1024,tData->offset)) == -1){
        printf("pwrite %d failed\n",tData->threadid);
    }else{
        printf("线程 %d pwrite 操作成功!\n",tData->threadid);
        //printf("线程 %d write buf is:  %s\n",tData->threadid,tData->buf_string);
    }
    
}


void main(int argc, char *argv[]){

    //构建XX MB大小的字符串
    char *buf_string = getBufString(1);

    //获取程序开始执行时间
    struct timeval start, end;
    float time_use=0;
    gettimeofday(&start,NULL);
    
    //打开设备文件
    const char* pathname="/dev/sdb";
    int fd=open(pathname,O_WRONLY);
    if (fd==-1) {
        printf("%s",strerror(errno));
    }


    //创建线程
    pthread_t Pthread[NUM_Threads];
    THDATA index[NUM_Threads];
    int i,ret;
    for ( i = 0; i <NUM_Threads; i++)
    {
        printf("main() : 创建线程 %d \n",i);
        index[i].threadid = i;
        index[i].fd = fd;
        index[i].blocksize = 1;
        index[i].offset = 128 + i*(1024*1024+2);
        index[i].buf_string = buf_string;
        ret = pthread_create(&Pthread[i],NULL, pwritedisk,(void *)&index[i]);
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


    close(fd);

    //获取程序结束时间并计算程序执行时间
    gettimeofday(&end,NULL);
    time_use=(end.tv_sec-start.tv_sec)*1000000+(end.tv_usec-start.tv_usec);//微秒
    printf("time_use is %.10f us\n",time_use);
    //printf("when the blocksize is %d MB, the read speed is %.10f MB/s\n");

    printf("the main() thread end!\n");

    free(buf_string);

}