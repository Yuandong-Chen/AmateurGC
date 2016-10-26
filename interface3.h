#ifndef __INTERFACE3_H_
#define __INTERFACE3_H_
#include <pthread.h>
#include <ctype.h>

pthread_mutex_t gc_lock; /* largest lock for __gc_calloc */

void *__gc_calloc(size_t size);
int __gc_init();
void __gc_exit();

#endif /* __INTERFACE3_H_ */
