CFLAGS = -g -Wall -Werror -std=gnu99



prog: performConnection.o prolog.o config.o sharedmemory.o spielfeldausgabe.o
	gcc $(CFLAGS) -o client prolog.o performConnection.o config.o sharedmemory.o spielfeldausgabe.o


sharedmemory.o: sharedmemory.c sharedmemory.h
	gcc $(CFLAGS) -c sharedmemory.c -D_XOPEN_SOURCE=600	
performConnection.o: performConnection.c performConnection.h
	gcc $(CFLAGS) -c performConnection.c
prolog.o: prolog.c
	gcc $(CFLAGS) -c prolog.c
config.o: config.c config.h
	gcc $(CFLAGS) -c config.c
spielfeldausgabe.o: spielfeldausgabe.c spielfeldausgabe.h
	gcc $(CFLAGS) -c spielfeldausgabe.c
clean:
	rm -f client prog1 *.o