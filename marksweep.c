#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "interface3.h"
#include "interface2.h"
#include "interface.h" /* for invisitable struct */
#include "marksweep.h"

extern RBRoot *close_rbtree_root;

static void mark(ptr p)
{
	
	//sleep(1);	
	if(!p)
	   return;
	
	ptr b;
	int i,len;
	if((b =(unsigned int)__rb_find_node(p)) == 0)
		return;

	
	if(b == (unsigned int)close_rbtree_root) {
		fprintf(stderr, "Warning: sth points to stack root\n");
		return;
	}

	if(GET_MARK(b))
	    return;

	//sleep(3);
	SET_MARK(b, MARK);
	len = GET_SIZE(b) - 2*DSIZE - WSIZE;
	for (i = 0; i < len; i += 4)
	{
		mark(*((unsigned int *)(unsigned int)(b+4+i)));
	}

	return;
}

int link_insert(Rn *root, Type data)
{
	Ln *newln = (Ln *)__malloc(sizeof(Ln));
	newln->data = data;
	newln->next = root->node;
	root->node = newln;
	return 0;
}

void link_destroy(Ln *node)
{
	if(node == NULL)
		return;

	if(node->next !=NULL){
		link_destroy(node->next);
	}
	__free(node);
	node->next = NULL;
	return;
}

static void postorder_sweep(RBTree tree, Rn *root)
{
    if(tree != NULL)
    {
        postorder_sweep((RBTree)GET_LEFT(tree), root);
        postorder_sweep((RBTree)GET_RIGHT(tree), root);
        if(GET_MARK(tree)){
        	SET_MARK(tree,UNMARK);
	}
        else if(GET_ALLOC(tree)){ 
        	link_insert(root,(Type)(tree+1));
	}
    }
}

static void sweep()
{
	Ln *curr = NULL;
	Rn *root = (Rn *)__malloc(sizeof(Rn));
	root->node = NULL;
	postorder_sweep(close_rbtree_root->node, root);
	
	curr = root->node;
	while(curr)
	{	
		__rb_free((RBTree)(curr->data));
		curr = curr->next;
	}
	
	link_destroy(root->node);
	int i = 0;
	curr = root->node;
	
	__free(root);
	root->node = NULL;
}

static void mark_from_region(ptr *begin, ptr *end)
{
	ptr *curr = begin;
	ptr tofind = 0;
	for (; curr < end; curr+= 1)
	{
		if((tofind =*curr) == 0)
		    continue;
		//sleep(2);
		//printf("tomark %p %p\n", curr,*curr);
		//print_info();
		
		mark(tofind);
	}
}

static unsigned int stack_bottom = 0;
static unsigned int isinit = 0;
extern char _end[];
extern char _etext[];
extern char** environ;

void gc_collect()
{
	unsigned int stack_top;
	unsigned int _eax_,_ebx_,_ecx_,_edx_,_esi_,_edi_;

    asm volatile ("movl    %%ebp, %0" : "=r" (stack_top));

    asm volatile ("movl    %%eax, %0" : "=r" (_eax_));
    asm volatile ("movl    %%ebx, %0" : "=r" (_ebx_));
    asm volatile ("movl    %%ecx, %0" : "=r" (_ecx_));
    asm volatile ("movl    %%edx, %0" : "=r" (_edx_));
    asm volatile ("movl    %%esi, %0" : "=r" (_esi_));
    asm volatile ("movl    %%edi, %0" : "=r" (_edi_));

    mark(_eax_);
    mark(_ebx_);
    mark(_ecx_);
    mark(_edx_);
    mark(_esi_);
    mark(_edi_);
    //printf("etxt end: %p %p\n",_etext, _end);
    int i=stack_top+4;

    mark_from_region((ptr *)((char *)stack_top + 4),(ptr *)(stack_bottom));
    mark_from_region((ptr *)((char *)_etext + 6),(ptr *)(_end));

    sweep();
}

void *__gc_calloc(size_t size)
{
	double ratio = (double)(rvolume) / MAX_VOLUME;
	//printf("e****ratio: %f\n",(double)rvolume / MAX_VOLUME);
	if(ratio > 0.35){
	    //printf("s****ratio: %f\n",(double)volume / MAX_VOLUME);
	   gc_collect();
	   
	}
	
	//sleep(1);
	return __rb_calloc(size);
}

int __gc_init()
{
	unsigned int stack_top;
	//signal(SIGSEGV,SIG_IGN);
	unsigned int curr;
	asm volatile ("movl %%ebp, %0" : "=r" (stack_top));
	curr = stack_top;
	while(*(unsigned int *)curr != (unsigned int)environ)
	{
		curr++;
	}
	stack_bottom = curr;
	//signal(SIGSEGV,SIG_DFL);
	return __rb_init();
}

void __gc_exit()
{
	__rb_exit();
}
