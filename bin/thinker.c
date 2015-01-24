#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <limits.h>
#include <netinet/in.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "sharedmemory.h"
#include "spielfeldausgabe.h"
#include "thinker.h"
#define BUFFR 1024

void SetorDelete(char* nextMove, SharedMem* shm, Spielfeldshm* spielfeld, int availableFlag) {

    //printspielfeld(spielfeld);
	srand(time(NULL));
	int i = rand() % 3;
	int j = rand() % 8;
	char* secondIndex = malloc(sizeof(char) * BUFFR);

	bzero(nextMove, BUFFR);
	bzero(secondIndex, BUFFR);

	if (availableFlag) {
    	while (spielfeld->feld[i][j] != -1) {
    		i = rand() % 3;
    		j = rand() % 8;
   	   }
	} else {
    	while ((spielfeld->feld[i][j] == -1) || (spielfeld->feld[i][j] == shm->eigspielernummer)) {
       		i = rand() % 3;
    		j = rand() % 8;
        }
	}

	switch (i) {
    	case 0:
    		strcat(nextMove, "A");
    		break;
    	case 1:
    		strcat(nextMove, "B");
    		break;
    	case 2:
    		strcat(nextMove, "C");
            break;
	}

	sprintf(secondIndex, "%d", j);
	strcat(nextMove, secondIndex);

    printf("noch %d Steine\n",spielfeld->steineverfuegbar);

    free(secondIndex);

}

void thinkMove(char* nextMove, SharedMem* shm, Spielfeldshm* spielfeld) {

	printf("think move\n");
	srand(time(NULL));
	int i;
	int j;
	int iNeu = -1;
	int jNeu = -1;
	char *tempString = malloc(sizeof(char)*BUFFR);

	bzero(nextMove, BUFFR);
	bzero(tempString, BUFFR);

   	while (1) {

		i = rand() % 3;
		j = rand() % 8;
		
    	while (spielfeld->feld[i][j] != shm->eigspielernummer) {
    		i = rand() % 3;
        	j = rand() % 8;
    	}
		
    	if ((spielfeld->feld[i][j+1] == -1) && (j+1 < 8)) {
    		iNeu = i;
    		jNeu = j+1;
    		break;
    	}
		
        if (j-1 > 0) {
			if (spielfeld->feld[i][j-1] == -1) {
				iNeu = i;
    			jNeu = j-1;
    			break;
			}
        }
	
    	if ((spielfeld->feld[i+1][j] == -1) && (i+1 < 3) && (j == 1 || j == 3 || j == 5 || j == 7)) {
    		iNeu = i+1;
    		jNeu = j;
    		break;
    	}

    	if (i-1 > 0) {
    		if ((spielfeld->feld[i-1][j] == -1) && (j == 1 || j == 3 || j == 5 || j == 7)) {
    			iNeu = i-1;
    			jNeu = j;
    			break;
    		}
        }

	}

	switch (i) {
    	case 0:
    		strcat(nextMove, "A");
    		break;
    	case 1:
    		strcat(nextMove, "B");
    		break;
    	case 2:
    		strcat(nextMove, "C");
    		break;
    }

	sprintf(tempString, "%d", j);
	strcat(nextMove, tempString);
	strcat(nextMove, ":");

	switch (iNeu) {
    	case 0:
    		strcat(nextMove, "A");
    		break;
    	case 1:
    		strcat(nextMove, "B");
    		break;
    	case 2:
        	strcat(nextMove, "C");
       		break;
    }
	
	sprintf(tempString, "%d", jNeu);
	strcat(nextMove, tempString);

    free(tempString);

}

int think(int pipeWrite, SharedMem* shm, Spielfeldshm* spielfeld) {

    char* nextMove = malloc(sizeof(char) * BUFFR);
	
    bzero(nextMove, BUFFR);

    if(spielfeld->capture_flag > 0) {

        SetorDelete(nextMove, shm, spielfeld, 0);

    } else if(spielfeld->steineverfuegbar >0) {

        SetorDelete(nextMove, shm, spielfeld, 1);

        spielfeld->steineverfuegbar = spielfeld->steineverfuegbar - 1;

    } else {

        thinkMove(nextMove, shm, spielfeld);

    }

    printf("MOVE %s\n",nextMove);

    if (write(pipeWrite, nextMove, PIPE_BUF) == -1) {
        perror("\nFehler beim Schreiben in die Pipe");
        return EXIT_FAILURE;
    }

    shm->think_flag = 0;

    free(nextMove);
    return EXIT_SUCCESS;

}