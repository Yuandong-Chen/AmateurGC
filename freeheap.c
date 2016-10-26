
#include "freeheap.h"

static char *freeheap_start;
static char *freeheap_brk;
static char *freeheap_max_addr;

void freeheap_init()
{
	if((freeheap_start = (char *)calloc(MAX_HEAP,sizeof(char))) == NULL)
	{
		fprintf(stderr, "Error: Cannot malloc 20 MB.\n");
	}

	freeheap_brk = freeheap_start;
	freeheap_max_addr = freeheap_start + MAX_HEAP;
}

void *freeheap_sbrk(int incr)
{
	char *old_brk = freeheap_brk;

    if ( (incr < 0) || ((freeheap_brk + incr) > freeheap_max_addr)) {
		errno = ENOMEM;
		fprintf(stderr, "Error: freeheap_sbrk failed. Ran out of memory.\n");
		return (void *)-1;
    }
    
    freeheap_brk += incr;
    return (void *)old_brk;
}

void freeheap_exit()
{
	if(freeheap_start != NULL){
		free(freeheap_start);
	}

	freeheap_start = NULL;
	freeheap_brk = NULL;
	freeheap_max_addr = NULL;
}

void *freeheap_lo()
{
    return (void *)freeheap_start;
}


void *freeheap_hi()
{
    return (void *)(freeheap_brk - 1);
}


size_t freeheap_size() 
{
    return (size_t)((void *)freeheap_brk - (void *)freeheap_start);
}


size_t freeheap_pagesize()
{
    return (size_t)getpagesize();
}

