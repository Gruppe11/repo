#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <limits.h>
#include <netinet/in.h>
#include "thinker.h"
#include "sharedmemory.h"

void think(int pipeWrite) {
	//Setzt einfach nur A1
	char puffer1[PIPE_BUF] = "A1";
	
	printf("puffer1: %s\n", puffer1);	

	write(pipeWrite, puffer1, PIPE_BUF);
}
