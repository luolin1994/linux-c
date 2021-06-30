#define _XOPEN_SOURCE 500
#include <stdint.h>
#include <fcntl.h>
#include <linux/fs.h>
#include <errno.h> 
#include <stdio.h>
#include <stdlib.h>
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

void main(){
    FILE* fp = NULL;
    char* buf = getBufString(16);

    // 以附加方式打开可读/写的文件, 如果没有此文件则会进行创建，然后以附加方式打开可读/写的文件
    //函数fopen_s不可用？？？
    fp = fopen("demo.txt","a+");
    fputs(buf,fp);

    free(buf);
    fclose(fp);
    

    
}

