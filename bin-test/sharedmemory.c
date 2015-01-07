#include <stdio.h> 
#include <stdlib.h> 
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "sharedmemory.h"

/* Implementierung des Shared Memory */

// shm anlegen
int initshm(int size) {

	int shmID;
	int shmSize = size;
	shmID = shmget(IPC_PRIVATE, shmSize, IPC_CREAT | IPC_EXCL | 0775);
	return shmID;

}

// Funktion zum Binden des shm
void bindshm(int shmID, shm* shmpointer) {

	// shm binden
	shmpointer = shmat(shmID,0,0);

}

void bindfeld(int feldID, spielfeld* shmpointer) {

	// shm binden
	shmpointer = shmat(feldID,0,0);

}

// Funktion zum Löschen des shm*
int delshm(int ID) {

	int delete;
	delete = shmctl(ID, IPC_RMID, NULL);
	return delete;

}