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

//顺序读取设备文件
//usage is:  函数名   块大小(MB)   读取次数    初始偏移值
void main(int argc, char *argv[]){

    if(argc != 4){
        printf("usage is:  函数名   块大小(MB)   读取次数    初始偏移值\n");
        exit(1);
    }

    //获取参数
    int blocksize = atoi(argv[1]);  //convert a string to an integer
    int num = atoi(argv[2]);
    int initial_offset = atoi(argv[3]);
    printf("blocksize is %d MB\n",blocksize);
    printf("the number of write times is  %d\n",num);
    printf("the initial offset is %d\n",initial_offset);

    //获取程序开始执行时间
    struct timeval start, end;
    float time_use=0;
    gettimeofday(&start,NULL);
    
    const char* pathname="/dev/sdb";
    int fd=open(pathname,O_RDONLY);
    if (fd==-1) {
        printf("%s",strerror(errno));
    }


    //顺序读取
    int i;
    for(i = 0;i < num ;i++){
        char *buf= (char *)malloc(blocksize * 1024 *1024* sizeof(char) + sizeof(char)); 
        int offset = initial_offset + i*(blocksize*1024*1024);
        ssize_t ret = 0;
        if((ret = pread(fd,buf,blocksize * 1024 *1024,offset)) == -1){
            printf("pread failed");
        }else{
            printf("第%d次pread success!\n",i);
            //printf("read buf is:  %s\n", buf);
        }
        free(buf);
    }
  
    close(fd);


    //获取程序结束时间并计算程序执行时间
    gettimeofday(&end,NULL);
    time_use=(end.tv_sec-start.tv_sec)*1000000+(end.tv_usec-start.tv_usec);//微秒
    printf("time_use is %.10f us\n",time_use);
    printf("when the blocksize is %d MB, the read speed is %.10f MB/s\n",blocksize,(blocksize*num)/((double)time_use/1000000));



}