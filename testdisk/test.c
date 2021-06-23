#include <stdlib.h>
#include <stdio.h>
#include <math.h>


void main(int argc, char*argv[]){
    double num = 92, inter;
    double fradd = modf(num,&inter);

    int i;
    for ( i = 0; i < inter; i++){
        printf("%d",i);
    }
    printf("\n");

    printf("inter: %lf\nfradd: %lf\n",inter,fradd);

    if(0 == fradd){
        printf("is 0");
    }
    double num = atof(argv[1]);
    printf("%f\n",num);
}