#include <stdlib.h>
#include <stdio.h>
#include <math.h>


void main(int argc, char*argv[]){
    // double num = 92, inter;
    // double fradd = modf(num,&inter);

    // int i;
    // for ( i = 0; i < inter; i++){
    //     printf("%d",i);
    // }
    // printf("\n");

    // printf("inter: %lf\nfradd: %lf\n",inter,fradd);

    // if(0 == fradd){
    //     printf("is 0");
    // }
    // // double num1= atof(argv[1]);
    // // printf("%f\n",num1);

    // printf("%d",sizeof(long));
    long initial =128;
    long blocksize =16;
    long i;
    long  offset;
    long num = 3254780226;
    printf("num is %ld\n",num);
    printf("sizeof is %ld\n",__LONG_MAX__);
    for(i =0 ; i< 200 ;i++){
        offset = initial+ i*(blocksize*1024*1024+1);
        printf("%d the offset is %ld\n",i,offset);
    }
}