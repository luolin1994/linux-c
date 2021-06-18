//基于线程的并发编程
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
/*
 * pthread库不是Linux系统默认的库，连接时需要使用库libpthread.a,
 * 在使用pthread_create创建线程时，在编译中要加-lpthread参数:
 * gcc createThread.c -lpthread -o createThread
 * ./createThread.o
 * 加上上面两句以后编译成功，并输出结果
 * */
//GDB调试， 在编译时要将调试信息加到可执行文件，使用编译器（cc/gcc/g++）的 -g 参数可以做到这一点，例如：gcc -g hello.c -o hello
#define NUM_Threads 5



/*

无参数传递的线程并发编程


*/
//线程运行函数，指针函数, void  *为无类型指针
void *PrintHello(void *arg){
    printf("hello,world of thread in c!\n");
    return 0;
}
void testPrintHello(){
    int i;
    int ret;
    //定义线程的id变量
    pthread_t tids[NUM_Threads];

    for ( i = 0; i < NUM_Threads; i++)
    {
        //参数依次是： 创建的线程id，线程参数，调用的函数，传入的函数参数
        ret = pthread_create(&tids[i],NULL,PrintHello,NULL);
        if(ret != 0){
            printf("pthread_create error: error_code = \n");
        }
    }
    //等各个线程推出后，进程才结束
    pthread_exit(NULL);
}




/*

简单参数传递的线程并发编程

*/
void *PrintHelloId(void *threadid){
     // 对传入的参数进行强制类型转换，由无类型指针变为整形指针，然后再读取
    int tid = *((int *)threadid);
    printf("hello world, Thread %d\n",tid);
    return 0;
}
void testPrintHelloId(){
    pthread_t pthreads[NUM_Threads];
    int i ,rc;

    int indexes[NUM_Threads];

    for ( i = 0; i < NUM_Threads; i++)
    {
        printf("main(): 创建线程 %d \n",i);
        indexes[i] = i;
        //在indexes传入参数的时候必须转换为无类型指针
        rc = pthread_create(&pthreads[i],NULL,PrintHelloId,(void *)&indexes[i]);
        if (0 != rc){
            printf("Error: 无法创建线程！\n");
        }
    }

    pthread_exit(NULL);
    
}



/*

结构体参数传递的线程并发编程


*/
typedef struct thread_data
{
    int threadid;
    char message;
}THDATA, *PTHDATA;

void  * PrintHelloSTR(void * pthreadid){
    PTHDATA tid = (PTHDATA)pthreadid;
    printf("This is Pthread : %d ;info : %c \n",tid->threadid, tid->message);
}

void testPrintHelloSTR(){
    pthread_t Pthread[NUM_Threads];
    THDATA index[NUM_Threads];
    int i , ret;

    for ( i = 0; i < NUM_Threads; i++)
    {
        printf("main() : 创建线程 %d \n",i);
        index[i].threadid = i;
        index[i].message = 'A'+i%10;
        ret = pthread_create(&Pthread[i],NULL, PrintHelloSTR,(void *)&index[i]);
        if (0 != ret){
            printf("Error: 创建线程失败！\n");
        }
    }
    pthread_exit(NULL);
    
}

/*

基于线程的并发编程,连接或分离线程
待学习

*/
void *PrintHelloJoin(void * pthreadid){
    int tid = *((int *)pthreadid);
    printf("Sleeping in thread %d ,...exiting \n",tid);
    return 0;
}
void testPrintHelloJoin(){
    int i, ret;
    pthread_t Pthread[NUM_Threads];
    pthread_attr_t attr; // 定义线程属性
    void * status;
    int index[NUM_Threads];

    // 初始化并设置线程为可连接
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);//PTHREAD_CREATE_DETACHED

    for (i=0; i<NUM_Threads; i++){
        printf("main() : 创建线程 %d \n",i);
        index[i] = i;
        ret = pthread_create(&Pthread[i], NULL, PrintHelloJoin, (void *)&index[i]);
    }

    // 删除属性，并等待其他线程
    pthread_attr_destroy(&attr);
    for (i=0; i<NUM_Threads; i++){
        ret = pthread_join(Pthread[i], status);
        if (0 != ret){
            printf("Error: unable to join,%d\n",ret);
        }
        printf("main(): complete thread id : %d",i);
        printf("exiting with status : %p\n",status);

    }
    printf("main() : program exiting.\n");
    pthread_exit(NULL);

}



int main(){
    //testPrintHello();//无参数传递的线程并发编程
    //testPrintHelloId();//简单参数传递的线程并发编程
    testPrintHelloSTR();//结构体参数传递的线程并发编程
    //testPrintHelloJoin();//基于线程的并发编程,连接或分离线程

    return 0;
    
}
