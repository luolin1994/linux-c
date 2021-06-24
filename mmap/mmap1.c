/**
 * 使用mmap将用户选择的文件输出到标准输出
 * mmap函数把一个文件或一个Posix共享内存区对象映射到调用进程的地址空间。
 * 使用方法：
 * #include <sys/mman.h>
 * void * mmap (void *addr, size_t len, int prot,int flags, int fd, off_t offset)
 * addr：指定被映射到进程空间内的起始地址，通常设为 NULL，代表让系统自动选定地址，映射成功后返回该地址。
 * len：映射到调用进程地址空间中的字节数。
 * prot：内存映射区域的保护方式。常用 PROT_READ | PROT_WRITE 
         PROT_EXEC 映射区域可被执行
         PROT_READ 映射区域可被读取
         PROT_WRITE 映射区域可被写入
         PROT_NONE 映射区域不能存取
 * flags：MAP_SHARED 和 MAP_PRIVATE 必须指定一个，其他可选。
         MAP_SHARED 调用进程对被映射数据所作修改对于共享该对象的所有进程可见，并且改变其底层支撑(物理内存) 并不是改变内存数据就马上写回磁盘。这个取决于虚拟存储的实现。
         MAP_PRIVATE 调用进程对被映射数据所作的修改只对该进程可见，而不改变其底层支撑(物理内存) 
         MAP_FIXED 用于准确解释addr参数，从移植性考虑不应指定它，如果没有指定，而addr不是空指针，那么addr如何处置取决于实现。不为空的addr值通常被当作有关该内存区应如何具体定位的线索。可移植的代码应把addr指定为空指针，并且不指定MAP_FIXED
         MAP_ANON 匿名映射时用
 * fd：要映射到内存中的文件描述符。如果使用匿名内存映射时，即flags中设置了MAP_ANON，fd设为-1。
 * offset：文件映射的偏移量，通常设置为0，代表从文件最前方开始对应，offset必须是分页大小的整数倍（一般是4096的整数倍）。
 * */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

int main (int argc, char *argv[]) {

    struct stat sb;
    off_t len;
    char *p;
    int fd;
    if (argc <2) {
        fprintf (stderr, "usage: %s <file>\n",argv[0]);
        return 1;
    }
    fd = open(argv[1],O_RDONLY);
    if (fd == -1){
        perror ("open");
        return ;
    }
    if (fstat(fd, &sb) == -1){
        perror("fstat");
        return 1;
    }
    if (!S_ISREG(sb.st_mode)) {
        fprintf(stderr,"”%s is not a file\n", argv[1]);
        return 1;
    }

    //mmap
    p = mmap(0,sb.st_size,PROT_READ,MAP_SHARED,fd,0);
    if (p == MAP_FAILED) {
        perror ("mmap");
        return 1;
    }
    if (close (fd) == -1){
        perror("close");
        return 1;
    }
    for(len = 0; len < sb.st_size; len++){
        putchar(p[len]);
    }
    printf("\n");
    if ( munmap(p,sb.st_size) == -1) {
        perror ("munmap");
        return 1;
    }
    return 0;
    
}
