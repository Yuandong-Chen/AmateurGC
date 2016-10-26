#ifndef __UNORDERED_LIST_H_
#define __UNORDERED_LIST_H_

#include "freeheap.h"

#define WSIZE       4
#define DSIZE       8
#define CHUNKSIZE  (1<<12)

#define MAX(x, y)	((x) > (y)? (x) : (y)) 
#define PACK(size, alloc)	((size) | (alloc))

#define GETINT(p)       (*(unsigned int *)(p))
#define PUTINT(p, val)  (*(unsigned int *)(p) = (val))


#define GET_SIZE(p)  (GETINT(p) & ~0x7)
#define GET_MARK(p)  (GETINT(p) & 0x2)
#define GET_ALLOC(p) (GETINT(p) & 0x1)

#define HDRP(bp)	((char *)(bp) - WSIZE -DSIZE)
#define FTRP(bp)	((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE - DSIZE)

#define NEXT_BLKP(bp)	((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE -DSIZE)))
#define PREV_BLKP(bp)	((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE -DSIZE)))

#define GET_LIST_PREV(bp)	(*(unsigned int *)((char *)bp - DSIZE))
#define GET_LIST_SUCC(bp)	(*(unsigned int *)((char *)bp - WSIZE))

#define SET_LIST_PREV(bp, val)	(*(unsigned int *)((char *)bp - DSIZE) = (val))
#define SET_LIST_SUCC(bp, val)	(*(unsigned int *)((char *)bp - WSIZE) = (val))

#endif /* __UNORDERED_LIST_H_ */