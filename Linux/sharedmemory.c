#include <stdio.h> 
#include <stdlib.h> 
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "sharedmemory.h"
#define _XOPEN_SOURCE
/**
 * Implementierung der Shared Memory
 *
 */

//shm anlegen
int initshm(){

int shmID;
int shmSize = sizeof(struct shm);
shmID = shmget(IPC_PRIVATE, shmSize, IPC_CREAT | IPC_EXCL | 0775);

if( shmID == -1){
	perror("shm erstellen lief schief");
	return EXIT_FAILURE;
	}
else{
	return shmID;
	}

}
// Funktion zum binden des shm
struct shm* bindshm(int shmID){

struct shm* shmpointer;
// shm binden
shmpointer = shmat(shmID,0,0);

if (shmpointer==(struct shm*)-1){
	perror("Fehler bim binden des shm");
	
	}
return shmpointer;
}

// Funktion zum Löschen des shm*
void delshm(int shmID){

int delete;
delete = shmctl(shmID, IPC_RMID, NULL);

if(delete == -1){
	perror("Fehler bei der Löschen des shm");
	}


}














