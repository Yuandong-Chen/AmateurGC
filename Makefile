all: clean example

example: example.c unordered_list.c freeheap.c
	gcc -m32 -O1 example.c marksweep.c rbtree.c unordered_list.c freeheap.c -lpthread -o example

clean:
	-rm ./example;
