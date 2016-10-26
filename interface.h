#ifndef __INTERFACE_H_
#define __INTERFACE_H_

volatile unsigned int rvolume;
void *__malloc(size_t size);
void __free(void *bp);
int __init();
void __exit();

#endif /* __INTERFACE_H_ */
