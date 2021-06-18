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


void main(){

    struct timeval start, end;
    float time_use=0;
    gettimeofday(&start,NULL);

    const char* pathname="/dev/sdb";
    int fd=open(pathname,O_RDONLY);
    if (fd==-1) {
        printf("%s",strerror(errno));
    }

    char buf[1024] = {0};
    int offset = 32;
    ssize_t ret = 0;
    int count = 128;

    if((ret = pread(fd,buf,count,offset)) == -1){
        printf("pread failed");
    }else{
        printf("pread success!\n");
        printf("read buf is:  %s\n", buf);
    }

    close(fd);
    gettimeofday(&end,NULL);
    time_use=(end.tv_sec-start.tv_sec)*1000000+(end.tv_usec-start.tv_usec);//微秒
    printf("time_use is %.10f us\n",time_use);


}