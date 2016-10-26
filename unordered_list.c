
#include "unordered_list.h"
#include "interface.h"

static char *heap_listp = 0;
//static char* closelist[14];
static char* openlist[14];

static void *extend_heap(size_t words);
static void place(void *bp, size_t asize);
static void *find_fit(size_t asize);
static void *coalesce(void *bp);

static void list_insert(char* list[], void* bp);
static int list_remove(char* list[], void* bp);
static int get_list_blknum(char* list[], int index);

volatile unsigned int inMalloc = 0;
volatile unsigned int inFree = 0;

int sizetoindex(size_t size)
{
    int i = 0;
    size_t csize = 1;

    while(csize < size)
    {
        i++;
        if(i >= 13){
            return 13;
        }

        csize <<=1;
    }

    return i;
}

static int list_remove(char* list[], void* bp)
{

    int index = sizetoindex(GET_SIZE(HDRP(bp)));

    void *prev = (void *)GET_LIST_PREV(bp);
    void *succ = (void *)GET_LIST_SUCC(bp);
     /* (void *)0 === NULL */
    
    if(prev != (void *)0)
    {
        SET_LIST_SUCC(prev, (unsigned int)succ);
    }else{
        list[index] = (void *)succ;
    }

    if(succ != (void *)0){
        SET_LIST_PREV(succ, (unsigned int)prev);
    }

    return 1;
}

static int get_list_blknum(char* list[], int index)
{
    if(index < 0 || index > 13){
        //fprintf(stderr, "Error: no such index in the list\n");
        return -1;
    }

    int size = 0;
    char* bp = list[index];
    if(bp == NULL)
        return size;
    do {
        size++;
    }while((bp = (char *)GET_LIST_SUCC(bp)) != 0);

    return size;
}

static void list_insert(char* list[], void* bp)
{
     

    int index = sizetoindex(GET_SIZE(HDRP(bp)));
    void *curr = list[index];
    void *next = NULL;

    if(curr == NULL){
        list[index] = bp;
        goto END;
    }

    SET_LIST_PREV(bp, 0);
    SET_LIST_SUCC(bp, (unsigned int)curr);
    SET_LIST_PREV(curr,(unsigned int)bp);
    list[index] = bp;
    goto END;
    

    END:
    return;
}

void list_init()
{
	int i = 0;
	for(; i < 14; i++)
	{
		//closelist[i] = NULL;
        openlist[i] = NULL;
	}

    return;
}


int __init()
{
	rvolume = 0;
    freeheap_init();

	if((heap_listp = freeheap_sbrk(4*WSIZE)) == (void *)-1){
        fprintf(stderr, "Error: failed in gc_init.\n");
        return -1;
    }

	list_init();

	PUTINT(heap_listp, 0);                          /* Alignment padding */
    PUTINT(heap_listp + (1*WSIZE), PACK(DSIZE, 1)); /* Prologue header */ 
    PUTINT(heap_listp + (2*WSIZE), PACK(DSIZE, 1)); /* Prologue footer */ 
    PUTINT(heap_listp + (3*WSIZE), PACK(0, 1));     /* Epilogue header */
    heap_listp += (2*WSIZE);

    if (extend_heap(CHUNKSIZE/WSIZE) == NULL) {
        fprintf(stderr, "Error: gc_init failed in extend_heap.\n");
        return -1;
    }
	
    return 0;
}

static void *extend_heap(size_t words) 
{
    char *bp;
    size_t size;

    /* Allocate an even number of words to maintain alignment */
    size = (words % 2) ? (words+1) * WSIZE : words * WSIZE; //line:vm:mm:beginextend
    if ((long)(bp = freeheap_sbrk(size) + DSIZE) == -1+DSIZE) {
        fprintf(stderr, "Error: failed in extend_heap\n");
        return NULL;  
    } 

    /* Initialize free block header/footer and the epilogue header */
    PUTINT(HDRP(bp), PACK(size, 0));         /* Free block header */   //line:vm:mm:freeblockhdr
    PUTINT(FTRP(bp), PACK(size, 0));         /* Free block footer */   //line:vm:mm:freeblockftr
    SET_LIST_SUCC(bp, 0);
    SET_LIST_PREV(bp, 0);
    PUTINT(HDRP(NEXT_BLKP(bp)), PACK(0, 1)); /* New epilogue header */ //line:vm:mm:newepihdr

    /* Coalesce if the previous block was free */
    return coalesce(bp);                                          //line:vm:mm:returnblock
}

void __free(void *bp)
{
    if(bp < freeheap_lo() + 2*DSIZE || bp > freeheap_hi()) {
        fprintf(stderr, "Error: freed ptr out of range.\n");
        return;
    }

    while(inMalloc){printf("M:F\t%u:%u\n", inMalloc, inFree);}

    inFree = 1;
    //list_remove(closelist,bp);
    size_t size = GET_SIZE(HDRP(bp));
    if(GET_ALLOC(HDRP(bp)))
        rvolume -= size;

    PUTINT(HDRP(bp), PACK(size, 0));
    PUTINT(FTRP(bp), PACK(size, 0));
    SET_LIST_PREV(bp, 0);
    SET_LIST_SUCC(bp, 0);
    coalesce(bp);

    inFree = 0;
}

static void *coalesce(void *bp) 
{

    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));
    void *next = NEXT_BLKP(bp);
    void *prev = PREV_BLKP(bp);

    if (prev_alloc && next_alloc) {            /* Case 1 */
        //list_remove(closelist,bp);

        list_insert(openlist,bp);
        return bp;
    }

    else if (prev_alloc && !next_alloc) {      /* Case 2 */
        //list_remove(closelist,bp);

        list_remove(openlist,next);
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUTINT(HDRP(bp), PACK(size, 0));
        PUTINT(FTRP(bp), PACK(size, 0));
        SET_LIST_PREV(bp, 0);
        SET_LIST_SUCC(bp, 0);
        list_insert(openlist,bp);
    }

    else if (!prev_alloc && next_alloc) {      /* Case 3 */


        //list_remove(closelist,bp);

        list_remove(openlist,prev);
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        PUTINT(FTRP(bp), PACK(size, 0));
        PUTINT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
        SET_LIST_PREV(bp, 0);
        SET_LIST_SUCC(bp, 0);
        list_insert(openlist,bp);
    }

    else {                                     /* Case 4 */
        //list_remove(closelist,bp);

        list_remove(openlist,next);
        list_remove(openlist,prev);
        size += GET_SIZE(HDRP(PREV_BLKP(bp))) + 
        GET_SIZE(FTRP(NEXT_BLKP(bp)));
        PUTINT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        PUTINT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
        SET_LIST_PREV(bp, 0);
        SET_LIST_SUCC(bp, 0);
        list_insert(openlist,bp);
    }

    return bp;
}

void *__malloc(size_t size) 
{
    size_t asize;      /* Adjusted block size */
    size_t extendsize; /* Amount to extend heap if no fit */
    char *bp;      

    if (size == 0)
    	return NULL;

    /* Adjust block size to include overhead and alignment reqs. */
    if (size <= DSIZE)
        asize = 3*DSIZE;
    else
        asize = DSIZE * ((size + (2*DSIZE) + (DSIZE-1)) / DSIZE);

    while(inMalloc || inFree){ printf("M:F\t%u:%u\n", inMalloc, inFree);}

    inMalloc = 1;
    /* Search the free list for a fit */
    if ((bp = find_fit(asize)) != NULL) {  //line:vm:mm:findfitcall
        
	place(bp, asize);                  //line:vm:mm:findfitplace
        inMalloc = 0;
        return bp;
    }

    /* No fit found. Get more memory and place the block */
    extendsize = MAX(asize,CHUNKSIZE);                 //line:vm:mm:growheap1
    if ((bp = extend_heap(extendsize/WSIZE)) == NULL) {
        inMalloc = 0;
        return NULL;                                  //line:vm:mm:growheap2
    } 
         
    place(bp, asize);                                 //line:vm:mm:growheap3

    inMalloc = 0;
    return bp;
}

static void *find_fit(size_t asize)
{
    void *bp = NULL;
    int index = sizetoindex(asize);
    int end = 14;

    for (; index < end; index++) {
        bp = openlist[index];
        while((bp != NULL) && (GET_SIZE(HDRP(bp)) < asize))
        {
            bp = (void *)GET_LIST_SUCC(bp);
        }

        if(bp != NULL)
        {
            return bp;
        }
    }
    return NULL; 
}

void __exit()
{
    void freeheap_exit();
}

static void place(void *bp, size_t asize)
     /* $end mmplace-proto */
{
    size_t csize = GET_SIZE(HDRP(bp));   

    if ((csize - asize) >= (3*DSIZE)) {
        list_remove(openlist,bp);
        PUTINT(HDRP(bp), PACK(asize, 1));
        PUTINT(FTRP(bp), PACK(asize, 1));
        SET_LIST_PREV(bp, 0);
        SET_LIST_SUCC(bp, 0);
        //list_insert(closelist,bp);
        bp = NEXT_BLKP(bp);
        PUTINT(HDRP(bp), PACK(csize-asize, 0));
        PUTINT(FTRP(bp), PACK(csize-asize, 0));
        SET_LIST_PREV(bp, 0);
        SET_LIST_SUCC(bp, 0);
        list_insert(openlist,bp);
	rvolume += asize;
    }
    else {
        list_remove(openlist,bp);
        PUTINT(HDRP(bp), PACK(csize, 1));
        PUTINT(FTRP(bp), PACK(csize, 1));
        SET_LIST_PREV(bp, 0);
        SET_LIST_SUCC(bp, 0);
        //list_insert(closelist,bp);
	rvolume += csize;
    }
}

