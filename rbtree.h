#ifndef _RED_BLACK_TREE_H_
#define _RED_BLACK_TREE_H_
#include "interface.h"
#include "rbtree_common.h"

#define RED        0    // 红色节点
#define BLACK    1    // 黑色节点

//从NODE模块头定义当前的地址,之后再转为指向下一个4字节地址作为返回__gc_malloc地址;
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
#define SET_COLOR_AT_ADDR(p, val)	(PUTINT_AT_ADDR(p, \
									PACK(GET_SIZE_AT_ADDR(p), \
									(GET_COLOR_AT_ADDR(p) & val | val), \
									GET_MARK_AT_ADDR(p), \
									GET_ALLOC_AT_ADDR(p))))



#define HDRP(bp)	((char *)(bp) - WSIZE -DSIZE)

#define GET_LEFT(bp)	(*(unsigned int *)((char *)bp - DSIZE))
#define GET_RIGHT(bp)	(*(unsigned int *)((char *)bp - WSIZE))
#define GET_PARENT(bp)	(*(unsigned int *)((char *)bp))
#define GET_KEY(bp)		((unsigned int)((char *)bp - WSIZE - DSIZE))
#define GET_SIZE(bp)  	(GET_SIZE_AT_ADDR(HDRP(bp)))
#define GET_COLOR(bp)	(GET_COLOR_AT_ADDR(HDRP(bp)))
#define GET_ALLOC(bp)	(GET_ALLOC_AT_ADDR(HDRP(bp)))
#define GET_MARK(bp)	(GET_MARK_AT_ADDR(HDRP(bp)))

#define SET_LEFT(bp, val)	(*(unsigned int *)((char *)bp - DSIZE) = (val))
#define SET_RIGHT(bp, val)	(*(unsigned int *)((char *)bp - WSIZE) = (val))
#define SET_PARENT(bp, val)	(*(unsigned int *)((char *)bp) = (val))
#define SET_COLOR(bp, val)  (SET_COLOR_AT_ADDR(HDRP(bp), val))


// 创建红黑树，返回"红黑树的根"！
RBRoot* create_rbtree();

// 销毁红黑树
void destroy_rbtree(RBRoot *root);

// 将结点插入到红黑树中。插入成功，返回0；失败返回-1。
int insert_rbtree(RBRoot *root, Type key);

// 删除结点(key为节点的值)
void delete_rbtree(RBRoot *root, Type key);


// 前序遍历"红黑树"
void preorder_rbtree(RBRoot *root);
// 中序遍历"红黑树"
void inorder_rbtree(RBRoot *root);
// 后序遍历"红黑树"
void postorder_rbtree(RBRoot *root);

// (递归实现)查找"红黑树"中键值为key的节点。找到的话，返回Node；否则，返回NULL。
Node* rbtree_search(RBRoot *root, Type key);
// (非递归实现)查找"红黑树"中键值为key的节点。找到的话，返回0；否则，返回-1。
int iterative_rbtree_search(RBRoot *root, Type key);

// 返回最小结点的值(将值保存到val中)。找到的话，返回0；否则返回-1。
int rbtree_minimum(RBRoot *root, unsigned int *val);
// 返回最大结点的值(将值保存到val中)。找到的话，返回0；否则返回-1。
int rbtree_maximum(RBRoot *root, unsigned int *val);

// 打印红黑树
void print_rbtree(RBRoot *root);

#endif
