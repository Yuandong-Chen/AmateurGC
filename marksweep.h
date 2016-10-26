#ifndef __MARKSWEEP_H_
#define __MARKSWEEP_H_

#define UNMARK        0
#define MARK    1

#define WSIZE       4
#define DSIZE       8

//对于块头只有 set color 的能力
#define PACK(size, color, mark, alloc)	((size) | (color << 2) | (mark << 1) | (alloc))

#define GETINT_AT_ADDR(p)       (*(unsigned int *)(p))
#define PUTINT_AT_ADDR(p, val)  (*(unsigned int *)(p) = (val))
#define GET_SIZE_AT_ADDR(p)		(GETINT_AT_ADDR(p) & ~0x7)
#define GET_COLOR_AT_ADDR(p)	((GETINT_AT_ADDR(p) & 0x4) >> 2)
#define GET_ALLOC_AT_ADDR(p) 	(GETINT_AT_ADDR(p) & 0x1)
#define GET_MARK_AT_ADDR(p)		((GETINT_AT_ADDR(p) & 0x2) >> 1)
#define SET_MARK_AT_ADDR(p, val)	(PUTINT_AT_ADDR(p, \
									PACK(GET_SIZE_AT_ADDR(p), \
									GET_COLOR_AT_ADDR(p), \
									(GET_MARK_AT_ADDR(p) & val | val), \
									GET_ALLOC_AT_ADDR(p))))


#define HDRP(bp)	((char *)(bp) - WSIZE -DSIZE)

#define GET_LEFT(bp)	(*(unsigned int *)((char *)bp - DSIZE))
#define GET_RIGHT(bp)	(*(unsigned int *)((char *)bp - WSIZE))
#define GET_ALLOC(bp)	(GET_ALLOC_AT_ADDR(HDRP(bp)))
#define GET_SIZE(bp)  	(GET_SIZE_AT_ADDR(HDRP(bp)))
#define GET_MARK(bp)	(GET_MARK_AT_ADDR(HDRP(bp)))
#define SET_MARK(bp, val)  (SET_MARK_AT_ADDR(HDRP(bp), val))

typedef unsigned int ptr;
typedef unsigned int Type;

typedef struct linknode
{
	Type data;
	struct linknode *next;
} Ln;

typedef struct linkroot
{
	struct linknode *node;
} Rn;

int link_insert(Rn *root, Type data);
void link_destroy(Ln *node);
void gc_collect();

#endif /* __MARKSWEEP_H_ */