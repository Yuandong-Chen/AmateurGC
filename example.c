#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "interface3.h"

#define NUM 10000
#define TEST 1000
#define RANGE 100

int *a[NUM];
size_t size[NUM];

void* leak(void* a)
{
    //sleep(1);
    int i = 0;
    for(; i< NUM; i++){
	pthread_mutex_lock(&gc_lock);
        int* leak = (int *)__gc_calloc(size[i]);
	pthread_mutex_unlock(&gc_lock);
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    clock_t start, finish, end;
    double duration1, duration2;

    int j;
    int i;
    pthread_mutex_init(&gc_lock,NULL);
    for (i = 0; i < NUM; ++i)
    {
        size[i] = 1 + (size_t)(1.0*RANGE*rand()/(RAND_MAX+1.0));
        //printf("%u\n", (unsigned int)size[i]);
    }
    //sleep(10);
    __gc_init();
    
    pthread_t t1;
    pthread_create(&t1, NULL, leak, NULL);
    start = clock();

    for (j = 0; j < TEST; ++j)
    {
        for(i = 0; i <NUM; i++){
	    pthread_mutex_lock(&gc_lock);
            a[i] = (int *)__gc_calloc(size[i]);  
            *a[i] = i;
	    pthread_mutex_unlock(&gc_lock);               
        }
    }
    //sweep();
    pthread_join(t1,NULL);
    finish = clock();
    
    for (j = 0; j < TEST; ++j)
    {
        for(i = 0; i <NUM; i++){
            a[i] = (int *)malloc(size[i]);  
            *a[i] = i;    
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
