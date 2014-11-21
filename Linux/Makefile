CFLAGS = -g -Wall -std=c99 




prog: performConnection.o prolog.o 
	gcc $(CFLAGS) -o prog1  prolog.o performConnection.o 

performConnection.o: performConnection.c performConnection.h 
	gcc $(CFLAGS) -c performConnection.c  
prolog.o:  prolog.c 
	gcc $(CFLAGS) -c  prolog.c   

