memgrind: memgrind.o mymalloc.o
	gcc -o memgrind -fsanitize=address -g memgrind.o mymalloc.o

memgrind.o: memgrind.c mymalloc.h
	gcc -c -fsanitize=address -g memgrind.c

mymalloc.o: mymalloc.c mymalloc.h
	gcc -c -fsanitize=address -g mymalloc.c

clean:
	rm -f memgrind *.o