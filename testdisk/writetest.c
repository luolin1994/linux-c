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





//顺序写入
void main(int argc, char *argv[]){
    
    //构建XX MB大小的字符串
    char *buf_string = getBufString(16);
    printf("the length of buf_string is : %d MB\n",(strlen(buf_string)+128)/1024/1024);

    //获取程序开始执行时间
    struct timeval start, end;
    float time_use=0;
    gettimeofday(&start,NULL);
    
    const char* pathname="/dev/sdb";
    int fd=open(pathname,O_WRONLY);
    if (fd==-1) {
        printf("%s",strerror(errno));
    }

    int i;
    ssize_t ret;
    for(i = 0; i< 20; i++){
        int  offset = 128 + i*(16*1024*1024+4);
        if((ret = pwrite(fd,buf_string,16*1024*1024,offset)) == -1){
            printf("pwrite is error!");}
        else{
            printf("%d pwrite success\n ",i);
            //printf("the writed data is: %s\n",buf_string);
    }
    }  
    close(fd);


    //获取程序结束时间并计算程序执行时间
    gettimeofday(&end,NULL);
    time_use=(end.tv_sec-start.tv_sec)*1000000+(end.tv_usec-start.tv_usec);//微秒
    printf("time_use is %.10f us\n",time_use);
    printf("when the blocksize is 16MB, the write speed is %.10f MB/s\n",(16*20)/((double)time_use/1000000));
    free(buf_string);
    

}