CFLAGS = -g -Wall -std=gnu99 



prog: performConnection.o prolog.o sharedmemory.o
	gcc $(CFLAGS) -o client prolog.o performConnection.o sharedmemory.o

performConnection.o: performConnection.c performConnection.h 
	gcc $(CFLAGS) -c performConnection.c  
prolog.o: prolog.c 
	gcc $(CFLAGS) -c prolog.c   
sharedmemory.o: sharedmemory.c sharedmemory.h
	gcc $(CFLAGS) -c sharedmemory.c -D_XOPEN_SOURCE=600

clean:
	rm -f client prog1 *.o