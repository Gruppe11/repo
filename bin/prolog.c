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

#define GAMEKINDNAME "NMMOrris"
#define VERSION "1.0"

int exit_flag = 0;

/* Signalhandler zum Unterbrechen von pause() + SIGINT */
void prologHandler(int sig) {
	if (sig == SIGUSR1) {
		signal(SIGUSR1, prologHandler);
	}
	if (sig == SIGUSR2) {
		printf("\nProgramm beendet\n");
		exit_flag = 1;
	}
	if (sig == SIGINT) {
		exit_flag = 2;
	}
}

int main(int argc, char *argv[]) {

	char game_id[11];
	int fd[2]; // für Pipe
	pid_t pid;

	char* conf_datei = malloc(256);

	int spielfeld_flag = 0;
	Spielfeldshm * spielfeld;
	int shmID = 0;
	int shmSize;

	// Signalhandler
	if (signal(SIGINT, prologHandler) == SIG_ERR) {
		perror("\nUnable to create handler for SIGINT\n");
	}

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


	/**
	 * Abfrage ob Konfiguationsdatei als Kommandozeile übergeben wird.
	 * Wenn nicht dann Default.
	 */
	if(argc < 3) {
		strcpy(conf_datei, "client.conf");
		fprintf(stdout, "\nKeine Konfigurationsdatei angegeben: Verwende Default\n");
	}
	else {
		strcpy(conf_datei, argv[2]);
	}

	// Config-Datei einlesen und struct initialisieren
	configstruct = get_config(conf_datei);

	/* SHM anlegen und binden */
	// Spielfeld SHM anlegen
	SharedMem* shm = malloc(sizeof(SharedMem));
	shmSize = sizeof(SharedMem);
	shmID = initshm(shmSize);

	// Überprüfe shmID
	if (shmID < 1) {
		printf("Kein SHM vorhanden\n");
		return EXIT_FAILURE;
	}

	// SHM binden 
	shm = (SharedMem*) attachshm(shmID);

	// Überprüfe SHM auf Fehler
	if (shm == (void *) -1) {
		printf("Fehler beim Binden des SHM\n");
		return EXIT_FAILURE;
	}

	// Spielfeld SHM zerstören
	if (delshm(shmID) == -1) {
		fprintf(stderr, "\nFehler bei Zerstoerung des SHM\n");
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
			// Schreibseite im Connector schließen
			close(fd[1]);
			shm->connectorpid = getpid();

			/* Verbindung zu Gameserver aufbauen */
			
			// Prologphase der Kommunikation mit dem Server durchführen und testen
			if (performConnection(VERSION, game_id, shm, fd[0]) != 0) {
				fprintf(stderr, "\nSocket geschlossen\n");
				kill(getppid(),SIGUSR2);
				return EXIT_FAILURE;
			}

			printf("Socket geschlossen.\n");

			// shm entfernen + conf_datei freigeben
			shmdt(shm);
			free(conf_datei);

			return EXIT_SUCCESS;
	
		// Thinker
		default:
			// Leseseite im Connector schließen
			close(fd[0]);
			shm->thinkerpid = pid;

			// Signalhandler
			if (signal(SIGUSR1, prologHandler) == SIG_ERR) {
				perror("\nParent: Unable to create handler for SIGUSR1\n");
			}
			if (signal(SIGUSR2, prologHandler) == SIG_ERR) {
				perror("\nParent: Unable to create handler for SIGUSR2\n");
			}

			// Schleife, damit Elternprozess nicht nach einmal thinken beendet
			while(1) {
				// Warten auf SIGUSR1
				pause();

				/* Routine zum Beenden */
				if (exit_flag > 0) {

					if (exit_flag == 2) {
						printf("\nProgramm wird ordnungsgemäß beendet...\n");
					}

					// shm + spielfeld entfernen
					shmdt(shm);
					shmdt(spielfeld);

					// Warten auf Kindprozess
					wait(NULL);

					if (exit_flag == 2) {
						return EXIT_FAILURE;
					}

					return EXIT_SUCCESS;
				}

				/* Spielfeld SHM */
				if (spielfeld_flag == 0) {

					// Spielfeld SHM binden 
					spielfeld = (Spielfeldshm*) attachshm(shm->spielfeldID);

					// Überprüfe Spielfeld SHM auf Fehler
					if (spielfeld == (void *) -1) {
						printf("Fehler beim Binden des Spielfeld SHM\n");
						return EXIT_FAILURE;
					}

					// Spielfeld SHM zerstören
					if (delshm(shm->spielfeldID) == -1) {
						fprintf(stderr, "\nFehler bei Zerstoerung des Spielfeld SHM\n");
					}

					spielfeld_flag = 1;
				}
	
				// Ruft think() in thinker.c auf
				if (think(fd[1], shm, spielfeld) != 0) {
					fprintf(stderr, "\nFehler beim Denkprozess\n");
				}
			}
	}
}
