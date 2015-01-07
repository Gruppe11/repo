#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "sharedmemory.h"
#include "performConnection.h"
#include "config.h"
#include "thinker.h"


/**
 * Implementierung der ersten Protokollphase der
 * Kommunikation mit dem Gameserver
 */


// Zur Verbindung nötige Konstanten deklarieren
// -> sind jetz in client.conf
	//#define PORTNUMBER 1357
	//#define HOSTNAME "sysprak.priv.lab.nm.ifi.lmu.de"
// String-Konstanten evtl. als const char hostname[] deklarieren?
#define GAMEKINDNAME "NMMOrris"
#define VERSION "1.0"

void handler(int sig); 

int main(int argc, char *argv[]) {

	char game_id[11];
	//int status;
	int fd[2]; // für Pipe
	pid_t pid;
	char* conf_datei = malloc(256);
	//FILE *childRead, *parentWrite;


	// Game-ID als Kommandozeilenparameter auslesen und überprüfen
	if (argc < 2){
		fprintf(stderr, "\nGame-ID nicht eingegeben\n");
		return EXIT_FAILURE; 
 	}
	if (strlen(argv[1]) != 11) {
		fprintf(stderr, "\nGame-ID nicht 11-stellig\n");
		return EXIT_FAILURE;
	}
	strcpy(game_id, argv[1]);


	/* Abfrage ob Konfiguationsdatei als Kommandozeile übergeben wird.
	** Wenn nicht dann Default. */
	if(argc < 3) {
		strcpy(conf_datei, "client.conf");
		fprintf(stdout, "\nKeine Konfigurationsdatei angegeben: Verwende Default\n");
	}
	else {
		strcpy(conf_datei, argv[2]);
	}
	// Config-Datei einlesen und struct initialisieren
	configstruct = get_config(conf_datei);

	shm* shm = malloc(sizeof(shm));
	int shmID = 0;
	int shmSize = sizeof(shm);

	shmID = initshm(shmSize);

	if (shmID < 1) {
		printf("No SHM\n");		
		return EXIT_FAILURE;
	}

	/* SHM binden */
	bindshm(shmID, shm);

	/* Im Fehler shm -1 */
	if (shm == (void *) -1) {
		printf("Fehler binden shm");
		return EXIT_FAILURE;
	}
	
	if (delshm(shmID) == -1) {
			
		fprintf(stderr, "\nFehler bei Zerstoerung von shm\n");
	
	}
	
	
	/* 
	** Pipe erzeugen
	** fd[0] zum Lesen
	** fd[1] zu Schreiben
	*/
	if (pipe(fd) < 0) {
		fprintf(stderr, "Fehler beim erzeugen der Pipe");
		return EXIT_FAILURE;
	}

	/**
	* zweiten Prozess erstellen
	* Connector ist Kindprozess
	* Thinker ist Elternprozess 
	*/
  
	pid = fork();

	switch (pid) {

		case -1:
			perror("\nFehler bei Erstellung des Kindprozesses");
			return EXIT_FAILURE;

		// Connector
		case 0:
			/* Schreibseite im Connector schließen */
			close(fd[1]);
	
			printf("\nKindprozess Connector beginnt:\n");

			shm->connectorpid = pid;

			/* Verbindung zu Gameserver aufbauen */

			// Prologphase der Kommunikation mit dem Server durchführen und testen
			if (performConnection(VERSION, game_id, shm, fd[0]) != 0) {
				fprintf(stderr, "\nSocket geschlossen\n");
				return EXIT_FAILURE;
			}

			free(conf_datei);
			break;
	
		// Thinker
		default: 
			/* Leseseite im Connector schließen */
			close(fd[0]);

			shm->thinkerpid = pid;
			
			//Signalhandler
			if(signal( SIGUSR1, handler) == SIG_ERR) {
        			printf("Parent: Unable to create handler for SIGUSR1\n");
   			}

			// Schleife, damit Elternprozess nicht nach einmal thinken beendet
			while(1) {
			// Warten aus SIGUSR1
			pause();
			printf("Es wird gedacht!\n");
			// Ruft think() in thinker.c auf
			think(fd[1]);
			}
			
			// shm zerstören	
			shmdt(shm);
			
			
			break;

	}
  
	//printf("Socket geschlossen.\n");
	return EXIT_SUCCESS;

}

/* Signalhandler zum unterbrechen von pause(); */
void handler(int sig) {
	if (sig == SIGUSR1) {
		signal(SIGUSR1, handler);	
	}
}
