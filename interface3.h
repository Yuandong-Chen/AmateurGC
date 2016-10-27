#ifndef __INTERFACE3_H_
#define __INTERFACE3_H_
#include <pthread.h>
#include <ctype.h>

pthread_mutex_t gc_lock; /* unimplemented! DO NOT USE our gc in multi-thread programs! */

void *__gc_calloc(size_t size);
int __gc_init();
void __gc_exit();

#endif /* __INTERFACE3_H_ */
