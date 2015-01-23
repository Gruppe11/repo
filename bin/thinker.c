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

char *SetorDelete(SharedMem * shm, Spielfeldshm * spielfeld, int availableFlag) {

//printspielfeld(spielfeld);
    	srand(time(NULL));
    	int i = rand() % 3;
    	int j = rand() % 8;
    	char *nextMove1 = malloc(sizeof(char)*BUFFR);
    	char *secondIndex = malloc(sizeof(char)*BUFFR);

	bzero(nextMove1, BUFFR);
	bzero(secondIndex, BUFFR);



    	if(availableFlag){
        	while(spielfeld->feld[i][j] != -1){
            		i = rand() % 3;
            		j = rand() % 8;
      
	   	}
    	}
    	else {
        	while((spielfeld->feld[i][j] == -1) || (spielfeld->feld[i][j] == shm->eigspielernummer)){
           		i = rand() % 3;
            		j = rand() % 8;
		}
    	}



    	switch(i){
        	case 0:
            		strcat(nextMove1, "A");
            		break;
        	case 1:
            		strcat(nextMove1, "B");
            		break;
        	case 2:
            		strcat(nextMove1, "C");
    	}

    	sprintf(secondIndex, "%d", j);
    	strcat(nextMove1, secondIndex);

	printf("noch %d Steine\n",spielfeld->steineverfuegbar);

    	return nextMove1;
}

char *thinkMove(SharedMem * shm, Spielfeldshm * spielfeld) {
    	printf("think move\n");
    	srand(time(NULL));
    	int i;
    	int j;
    	int iNeu = -1;
    	int jNeu = -1;
    	char *nextMove2 = malloc(sizeof(char)*BUFFR);
    	char *tempString = malloc(sizeof(char)*BUFFR);

	bzero(nextMove2, BUFFR);
	bzero(tempString, BUFFR);

   	while(1){
		i = rand() % 3;
		j = rand() % 8;
		
        	while(spielfeld->feld[i][j] != shm->eigspielernummer){
		
	    		i = rand() % 3;
            		j = rand() % 8;
        	}
		
        	if((spielfeld->feld[i][j+1] == -1) && (j+1 < 8)){
            		iNeu = i;
            		jNeu = j+1;
            		break;
        	}
		
        	if(j-1 > 0){
			if(spielfeld->feld[i][j-1] == -1) {
				iNeu = i;
            			jNeu = j-1;
            			break;
			}
        	}
	
        	if((spielfeld->feld[i+1][j] == -1) && (i+1 < 3) && (j == 1 || j == 3 || j == 5 || j == 7)){
		
            		iNeu = i+1;
            		jNeu = j;
            		break;
        	}
		
		if(i-1 > 0){
			if((spielfeld->feld[i-1][j] == -1) && (j == 1 || j == 3 || j == 5 || j == 7)) {
				iNeu = i-1;
            			jNeu = j;
            			break;
			}
        	}
    	}

    	switch(i){
        	case 0:
            		strcat(nextMove2, "A");
            		break;
        	case 1:
            		strcat(nextMove2, "B");
            		break;
        	case 2:
            		strcat(nextMove2, "C");
            		break;
    }
	
    	sprintf(tempString, "%d", j);
    	strcat(nextMove2, tempString);
    	strcat(nextMove2, ":");

	switch(iNeu){
        	case 0:
            		strcat(nextMove2, "A");
            		break;
        	case 1:
            		strcat(nextMove2, "B");
            		break;
        	case 2:
            		strcat(nextMove2, "C");
           		break;
    	}
	
	sprintf(tempString, "%d", jNeu);
	strcat(nextMove2, tempString);
	
	return nextMove2;
}
void think(int pipeWrite, SharedMem * shm, Spielfeldshm * spielfeld) {


	char *nextMove3 = malloc(sizeof(char)*BUFFR);
	
	bzero(nextMove3, BUFFR);

	if(spielfeld->zuschlagendesteine > 0){

            	strncpy(nextMove3, SetorDelete(shm, spielfeld, 0), BUFFR-1);
	}
        else if(spielfeld->steineverfuegbar >0){

            	strncpy(nextMove3, SetorDelete(shm, spielfeld, 1), BUFFR-1);
		spielfeld->steineverfuegbar = spielfeld->steineverfuegbar -1;
        }
        else {

            	strncpy(nextMove3, thinkMove(shm, spielfeld), BUFFR-1);
        }
        printf("MOVE %s\n",nextMove3);
        write(pipeWrite, nextMove3, PIPE_BUF);
	
	shm->think_flag = 0;
}
