//多进程实验
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>

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
     * 但是输出了两个hello!，这是根Printf的缓冲机制有关了，printf函数实际上只是输出到了标准输出缓冲队列stdout上，
     * 并没有实实在在的打印到屏幕上，标准输出是行缓冲机制，也就是说，遇到\n，就会刷新缓冲区，输出到屏幕上。
     * 运行到fork时,缓冲里面的hello被子进程(child process)继承了,因此在子进程度stdout缓冲里面就也有了”hello!”。
     * 但是改为printf("hello!\n")也有两个hello!????????????????????????????????????????????
     * */
    printf("hello!\n");  //printf("hello!\n");
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
        atexit(print_exit);   //当程序正常终止时，调用指定的函数, 退出处理函数可以做一些清理工作,需要先登记才生效,退出处理函数保存在退出处理函数栈中(先进后出的原则)
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

/**
 * 测试waitpid(pid_t pid, int *status, int options)，指定某一个子进程
 * pid:
 *   < -1 回收指定进程组内的任意子进程
 *      -1 回收任意子进程
 *       0 回收和当前调用waitpid一个组的所有子进程
 *    > 0 回收指定ID的子进程
 *1. WIFEXITED(status) 这个宏用来指出子进程是否为正常退出的，如果是，它会返回一个非零值。
 * WEXITSTATUS(status) 当WIFEXITED返回非零值时，我们可以用这个宏来提取子进程的返回值
 * 2.WIFSIGNALED(status)为非0 表明进程异常终止
 * WTERMSIG(status)当WIFSIGNALED(status)为非0，可获取使得进程退出的信号编号
 * 3. WIFSTOPPED(status)为非0 表明进程处于暂停状态
 *     若上宏为真，此时可通过WSTOPSIG(status)获取使得进程暂停的信号编号
 * 4. WIFCONTINUED(status) 非0表示暂停后已经继续运行
 * */
int testWaitpid(){
    pid_t pid;
    pid = fork();
    if(pid < 0){
        perror("fork failed");
        exit(1);
    }
    if(pid == 0){
        int i;
        for(i=3; i>0;i--){
            printf("This is the child\n");
            sleep(1);
        }
        exit(3);
    }else{
        int stat_val;
        waitpid(pid,&stat_val,0);
        if(WIFEXITED(stat_val)) //若子进程正常退出
            printf("child exited with code %d\n",WEXITSTATUS(stat_val));
        else if(WIFSIGNALED(stat_val)) //若子进程异常退出
            printf("child terminated abnormally, signal %d\n",WTERMSIG(stat_val));
    }
    return 0;
}

/**
 * wait的测试，等待第一个终止的子进程
 * 
 * */
void testWait(){
    pid_t pid;
    int i;
    for (i = 0; i<3; i++){
        if( (pid = fork() ) == 0){
            break;
        }
    }
    if(i == 3){    //父进程，调用3次wait，即等待所有的3个子进程结束
        for (i = 0; i < 3; i++)
            wait(NULL);
        printf("the main process end\n");
    }else{
        printf("this is the child %d\n",i);
        sleep(1);
        exit(3);

    }


}


void main(){
    //testFork();
    //testFork2();
    //int t = testWaitpid();
    testWait();

}