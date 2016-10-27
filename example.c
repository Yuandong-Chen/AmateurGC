#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "interface3.h"

#define NUM 10000
#define TEST 1000
#define RANGE 100

int *a[NUM];
size_t size[NUM];

void leak(int i)
{
    int* leak = (int *)__gc_calloc(size[i]);
    return;
}

int main(int argc, char *argv[])
{
    clock_t start, finish, end;
    double duration1, duration2;

    int j;
    int i;

    for (i = 0; i < NUM; ++i)
    {
        size[i] = 1 + (size_t)(1.0*RANGE*rand()/(RAND_MAX+1.0));
    }

    __gc_init();
    
    start = clock();

    for (j = 0; j < TEST; ++j)
    {
        for(i = 0; i <NUM; i++){
	    
            a[i] = (int *)__gc_calloc(size[i]);  
            *a[i] = i;
	    leak(i);              
        }
    }
   
    finish = clock();
    
    for (j = 0; j < TEST; ++j)
    {
        for(i = 0; i <NUM; i++){
            a[i] = (int *)malloc(size[i]);  /* malloc might return NULL since we already reserve 40M for our GC */
            *a[i] = i;   /* warning: if malloc return NULL, this will cause segmentation fault! */ 
        }

        for(i = 0; i <NUM; i++){
            free(a[i]);
        }
    }

    end = clock();

    duration1 = (double)(finish - start) / CLOCKS_PER_SEC;
    duration2 = (double)(end - finish) / CLOCKS_PER_SEC;
    __gc_exit();

    printf("\t\ttime(s)\t\tfrequency(HZ)\n");
    printf("\t-------------------------------------------\n");
    printf("__gc_malloc\t%.4f\t\t%.2f\nmalloc\t\t%.4f\t\t%.2f\n", 
        duration1, 2*NUM*TEST/duration1, duration2, 2*NUM*TEST/duration2);
    return 0;
}
