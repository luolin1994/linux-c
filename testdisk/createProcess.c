//多进程实验
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>


/**
 * fork()
 * 复制父进程的进程空间来创建子进程,其中代码段是不会不复制到子进程的,而是共享,
 * ,在fork函数执行完毕后,则有两个进程，一个是子进程，一个是父进程
 * 在子进程中，fork函数返回0，在父进程中，fork返回子进程的进程ID,
 * 注意⚠️:如果父进程提前结束,那么子进程就会变成孤儿进程
 * 进程创建成功之后，父进程以及子进程都从fork() 之后开始执行
 * 创建子进程之后，子进程拷贝了父进程所有的变量，并且子进程修改变量的值，并不影响父进程的变量值。
 * */
void testFork(){
    //printf("+++process %d start running!ppid = %d\n",getpid(),getppid());
    /**
     * 进程创建成功之后，父进程以及子进程都从fork() 之后开始执行
     * 但是输出了两个hello!，因为
     * */
    printf("hello!\n");
    pid_t pid = fork();

    if(pid)   //父进程
    {
        printf("parent:process %d start running!ppid = %d\n",getpid(),getppid());

    }
    else
    {
        printf("child:process %d start running!ppid = %d\n",getpid(),getppid());
        exit(0);
    }
    exit(0);
}

/**
 * fork()测试2
 * 创建子进程之后，子进程拷贝了父进程所有的变量，并且子进程修改变量的值，并不影响父进程的变量值。
 * */
void print_exit(){
    printf("the exit pid:[%d] \n",getpid() );
}
void testFork2(){
    char * sMatch;
    pid_t pid, child_pid;
    char *provList[5] ={"taskFace","taskObj","taskAction","taskHat","taskOther"};
    printf("Main process id = %d\n",getpid());

    int i;
    for(i = 0; i<5;i++){
        sMatch = provList[i];
        atexit(print_exit);   //退出处理函数可以做一些清理工作,需要先登记才生效,退出处理函数保存在退出处理函数栈中(先进后出的原则)
        pid = fork();

        // (*hsq*)子进程退出循环，不再创建子进程，全部由主进程创建子进程，这里是关键所在
        if(pid == 0 || pid == -1){
            break;
        }

        if(pid == -1)
        {
            printf("Fail to fork!/n");
            exit(1);
        }
        else if (pid == 0)
        {
            // 这里写子进程处理逻辑
            printf("This is children process, id= %d, start to process %s\n",getpid(),sMatch);
            sleep(10);
            exit(0);
        }
        else
        {
            // 这里主进程处理逻辑
            printf("This is main process, id = %d,end to process %s\n",getpid(),sMatch);
            do{

                // WNOHANG 非阻塞 轮询 等待带子进程结束
                child_pid = waitpid(pid, NULL, WNOHANG);  
                if(child_pid != pid)
                {
                     printf("---- watpid error!\n");
                }
                printf("I am main progress.The pid progress has not exited!\n");
                sleep(2);
            }while (child_pid == 0); //返回 0 值： 参3传WNOHANG，并且子进程尚未结束。
            exit(0);
        }
    }

}


void main(){
    testFork();
    //testFork2();

}