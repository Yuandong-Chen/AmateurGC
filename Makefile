MYGCC := gcc -m32 -O1 -fno-stack-protector -fno-omit-frame-pointer
APPEND := marksweep.c rbtree.c unordered_list.c freeheap.c

all: clean example snake

example: example.c
	$(MYGCC) example.c $(APPEND) -o example 

snake: snake.c
	$(MYGCC) snake.c $(APPEND) -lcurses -o snake 

clean:
	-rm ./example ./snake;
