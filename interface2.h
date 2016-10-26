#ifndef __INTERFACE2_H_
#define __INTERFACE2_H_

#include "rbtree_common.h"

#define MAX_VOLUME (40*(1<<20))  /* 40 MB */

RBRoot *close_rbtree_root;

void *__rb_calloc(size_t size);
void __rb_free(void *bp);
int __rb_init();
void __rb_exit();

RBTree __rb_find_node(Type key);

void print_info();

#endif /* __INTERFACE2_H_ */
