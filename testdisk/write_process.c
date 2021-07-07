/**
 * 多进程写入磁盘
 * */
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdint.h>
#include <errno.h> 
#include <sys/time.h>
#include <math.h>

#define NUM_Process 2



//随机生成字符串，有长度限制，在栈内存上，生成num bytes 大小的随机字符串
void getRandString(char s[],int num){
    int i;
    char *str = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz,./;\"'<>?";
    char ss[2] = {0};
    int lstr = strlen(str);//计算字符串长度
    srand((unsigned int)time((time_t *)NULL));//使用系统时间来初始化随机数发生器
    for( i = 1; i <= num; i++){
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


void main(int argc, char *argv[]){
    if(argc != 4){
        printf("usage is:  函数名   块大小(MB)   单盘写入次数    初始偏移值\n");
        exit(1);
    }

     //获取参数
    long blocksize = atol(argv[1]);  //convert a string to long
    long count = atol(argv[2]);
    long initial_offset = atol(argv[3]);
    printf("blocksize is %ld MB\n",blocksize);
    printf("the number of single disk write times is  %ld\n",count);
    printf("the initial offset is %ld\n",initial_offset);

    //构建XX MB大小的字符串
    char *buf_string = getBufString(blocksize);

    //获取程序开始执行时间
    printf("get the start time!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    struct timeval start, end;
    float time_use=0;
    gettimeofday(&start,NULL);

    //打开设备文件
    int fd[3];
    const char* pathname1="/dev/sdf";
    const char* pathname2="/dev/sdg";
    //const char* pathname3="/dev/sdd";
    fd[0] = open(pathname1,O_WRONLY);
    fd[1] = open(pathname2,O_WRONLY);
    //fd[2] = open(pathname3,O_WRONLY);
    if (fd[0]==-1 || fd[1]==-1) {
        printf("%s\n",strerror(errno));
    }

    

    //创建3个进程，每个进程写一个磁盘
    pid_t pid;
    int i;
    for (i = 0; i<NUM_Process; i++){
        if( (pid = fork() ) == 0){
            break;
        }
    }

    if(i == NUM_Process){    //父进程，调用3次wait，即等待所有的3个子进程结束
        for (i = 0; i < NUM_Process; i++)
            wait(NULL);

        printf("close fd!!!!!!!!\n");
        int z;
        for(z =0; z < NUM_Process; z++)
        {
            close(fd[z]);
        }
        

        //获取程序结束时间并计算程序执行时间
        printf("get the end time!!!!!!!!!!!!!!!!!!!!!!!!!\n");
        gettimeofday(&end,NULL);
        time_use=(end.tv_sec-start.tv_sec)*1000000+(end.tv_usec-start.tv_usec);//微秒
        printf("time_use is %.10f us\n",time_use);
        //printf("when the blocksize is %d MB, the read speed is %.10f MB/s\n");
        printf("when the blocksize is %d MB, the NUM_Process is %d,the write speed is %.10f MB/s\n",
        blocksize,NUM_Process,(blocksize*count*NUM_Process)/((double)time_use/1000000));
        printf("the length of buf_string is : %d MB\n",(strlen(buf_string)));
        free(buf_string);
        printf("the main process end\n");
        

    }else{
        printf("process : %d 开始执行 \n",i);
        ssize_t ret;
        long j;
        for(j = 0;j< count;j++){
            long poffset = initial_offset + j*(blocksize*1024*1024);
            if((ret = pwrite(fd[i],buf_string,blocksize*1024*1024,poffset)) == -1){
                printf("process %d pwrite %d failed\n",i,j);
            }else{
                printf("process %d pwrite 第 %ld次操作成功!\n",i,j);
            }  
        }
        exit(3);

    }


    

}
