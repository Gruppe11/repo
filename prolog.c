#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define GAMEKINDNAME "NMMorris"
#define PORTNUMBER 1357
#define HOSTNAME "sysprak.priv.lab.nm.ifi.lmu.de"

int main(int argc, char *argv[]) {
	char player = '2';
	char gameId[15];

	//11-stellige Game-ID als Kommandozeilenparameter auslesen
	if (argc < 2) {
		printf("Keine Game-Id angegeben!\n");
		help();
		exit(EXIT_FAILURE);
	}
	strcpy(gameId,argv[1]);
	if(strlen(gameId) != 11) {
		printf("Game-Id muss 11-stellig sein!\n");
		help();
		exit(EXIT_FAILURE);
	}

	performConnection(GAMEKINDNAME, PORTNUMBER, HOSTNAME);
}

int performConnection(char *name, int port, char *host) {
	
}