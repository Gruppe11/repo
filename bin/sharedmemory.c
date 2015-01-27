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
void* attachshm(int shmID) {

	// shm binden
	void* shmpointer;
	shmpointer = shmat(shmID,0,0);
	return shmpointer;
}

// Funktion zum Löschen des shm
int delshm(int ID) {

	int delete;
	delete = shmctl(ID, IPC_RMID, NULL);
	return delete;
}
