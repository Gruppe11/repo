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
    int i = ((int) rand()) % 3;
    int j = ((int) rand()) % 8;
    char *nextMove = malloc(sizeof(char)*BUFFR);
    char *secondIndex = malloc(sizeof(char)*BUFFR);



    if(availableFlag){
        while(spielfeld->feld[i][j] != -1){
            i = ((int) rand()) % 3;
            j = ((int) rand()) % 8;
      
	   }
    }
    else {
        while((spielfeld->feld[i][j] == -1) || (spielfeld->feld[i][j] == shm->eigspielernummer)){
            i = ((int) rand()) % 3;
            j = ((int) rand()) % 8;

		}
    }



    switch(i){
        case 0:
            strcat(nextMove, "A");
            break;
        case 1:
            strcat(nextMove, "B");
            break;
        case 2:
            strcat(nextMove, "C");
    }

    sprintf(secondIndex, "%d", j);
    strcat(nextMove, secondIndex);

printf("noch%d Steine\n",spielfeld->steineverfuegbar);
spielfeld->steineverfuegbar = spielfeld->steineverfuegbar -1;
    return nextMove;
}

char *thinkMove(SharedMem * shm, Spielfeldshm * spielfeld) {
    printf("think move\n");
    srand(time(NULL));
    int i;
    int j;
    int iNeu = -1;
    int jNeu = -1;
    char *nextMove = malloc(sizeof(char)*BUFFR);
    char *tempString = malloc(sizeof(char)*BUFFR);

    while(1){
	i = ((int) rand()) % 3;
	j = ((int) rand()) % 8;
        while(spielfeld->feld[i][j] != shm->eigspielernummer){
	    i = ((int) rand()) % 3;
            j = ((int) rand()) % 8;
        }

        if((spielfeld->feld[i][j+1] == -1) && (j+1 < 8)){
            iNeu = i;
            jNeu = j+1;
            break;
        }
        if((spielfeld->feld[i][j-1] == -1) && (j-1 >= 0)){
            iNeu = i;
            jNeu = j-1;
            break;
        }
        if((spielfeld->feld[i+1][j] == -1) && (i+1 < 3) && (j == 1 || j == 3 || j == 5 || j == 7)){
            iNeu = i+1;
            jNeu = j;
            break;
        }
		if((spielfeld->feld[i-1][j] == -1) && (i-1 >= 0) && (j == 1 || j == 3 || j == 5 || j == 7)){
            iNeu = i-1;
            jNeu = j;
            break;
        }
    }



    switch(i){
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

    switch(iNeu){
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
	
	
	
    return nextMove;
}
void think(int pipeWrite, SharedMem * shm, Spielfeldshm * spielfeld) {


	char *nextMove = malloc(sizeof(char)*BUFFR);


	if(spielfeld->zuschlagendesteine > 0){

            strncpy(nextMove, SetorDelete(shm, spielfeld, 0), BUFFR-1);
        }
        else if(spielfeld->steineverfuegbar >0){

            strncpy(nextMove, SetorDelete(shm, spielfeld, 1), BUFFR-1);
        }
        else {

            strncpy(nextMove, thinkMove(shm, spielfeld), BUFFR-1);
        }
        printf("MOVE%s\n",nextMove);
        write(pipeWrite, nextMove, PIPE_BUF);
	
	shm->think_flag = 0;
}
