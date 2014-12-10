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


int main(int argc, char *argv[]) {

	char game_id[11];
	int sock, status;
	struct sockaddr_in host;
	int fd[2]; // für Pipe
	pid_t pid;
	char* conf_datei = malloc(256);

	/* Möglichkeit der Angabe einer eigenen Config-Datei als Kommandozeilenargument muss noch implementiert werden  */
	// Nachfolgend wird einfach default Config-Datei verwendet
	// Config-Datei einlesen und struct initialisieren
	strcpy(conf_datei, "client.conf");
	configstruct = get_config(conf_datei);

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

	shm *shm;
	int shmID;
	int shmSize = sizeof(shm);

	shmID = initshm(shmSize);

	if (shmID < 1) {
		printf("No SHM\n");		
		return EXIT_FAILURE;
	}
	
	/* SHM binden */
	shm = bindshm(shmID);

	/* Im Fehler shm  -1 */
	if (shm == (void *) -1) {
		printf("Fehler binden shm");
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

			printf("\nKindprozess Connector beginnt:\n");		
			
			shm->connectorpid = pid;
			
			/* Verbindung zu Gameserver aufbauen */

			// Hostname in IP Adresse übersetzen
			struct hostent* ip = gethostbyname(configstruct.hostname);
			if (ip == NULL) {
				printf("\nFehler beim Anfordern der IP\n");
				return EXIT_FAILURE;
			}

			// Benötigte Variablen der Struktur Host Werte zuweisen
			memcpy(&(host.sin_addr), ip->h_addr_list[0], ip->h_length);
			host.sin_family = AF_INET;
			host.sin_port = htons(atoi(configstruct.portnumber));

			// Verbindung aufbauen und überprüfen
			sock = socket(AF_INET, SOCK_STREAM, 0);
			if (connect(sock, (struct sockaddr*) &host, sizeof(host)) == 0) {
				printf("\nVerbindung hergestellt!\n");
			} else {
				perror("\nFehler beim Verbindungsaufbau");
				return EXIT_FAILURE;
			}

			// Prologphase der Kommunikation mit dem Server durchführen und testen
			if (performConnection(sock, VERSION, game_id, fd, shm) != 0) {
				close(sock);
				fprintf(stderr, "\nSocket geschlossen\n");
				return EXIT_FAILURE;
			}

			free(conf_datei);
			break;
	
		// Thinker
		default: 
				
				shm->thinkerpid = pid;
			
			// Warten bis Kindprozess Connector fertig
			if (wait(&status) == -1) {
				perror("\nFehler beim Warten auf Kindprozess");
				return EXIT_FAILURE;
			}
			if (WIFEXITED(status) == 0) {
				printf("\nKindprozess nicht korrekt terminiert\n");
				return EXIT_FAILURE;
			}

			// shm zerstören	
			shmdt(shm);
			if (delshm(shmID) == -1) {
				printf("\nFehler bei Zerstoerung von shm\n");
			}
			break;

	}
  
	close(sock);
	//printf("Socket geschlossen.\n");
	return EXIT_SUCCESS;

}