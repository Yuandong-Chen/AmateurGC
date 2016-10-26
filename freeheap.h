#ifndef __FREEHEAP_H_
#define __FREEHEAP_H_
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>
/*
 * Alignment requirement in bytes (either 4 or 8)
 */
#define ALIGNMENT 8

/*
 * Maximum heap size in bytes
 */
#define MAX_HEAP (40*(1<<20))  /* 40 MB */
void freeheap_init();
void *freeheap_sbrk(int incr);
void freeheap_exit();

void *freeheap_lo();
void *freeheap_hi();
size_t freeheap_size();
size_t freeheap_pagesize();

#endif /* __FREEHEAP_H_ */
