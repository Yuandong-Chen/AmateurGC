# csapp-gc-calloc
A toy C garbage collector based on csapp malloc lab

1.How to use it?
Testing environment: Ubuntu 12.04.5

Suppose you write a program like this one:

//#include <...>

//int main()
//{
//	...
//	void *hatetofree = (void *)malloc(...);
//
//	...
//
//	free(hatetofree);
//
//	return 0;
//}

then transfer it into the following:

//#include <interface3.h>
//#include <...>
//
//int main()
//{
//	gc_init(); /* to init gc */
//	...
//	void *hatetofree = (void *)gc_calloc(...);
//
//	...
//
//	/* please forget free(hatetofree); */
//
//	gc_exit(); /* to destroy gc */
//	return 0;
//}

Our tardy garbage collector will help you take care of it.

Another example, please see file "example.c"
> make; ./example;

2.Limits:
Space Limits: ONLY 14M effective memory.
Only support 0 up to 10000 allocated pointers.
Conservative C collector.
