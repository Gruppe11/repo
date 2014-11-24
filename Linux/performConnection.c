#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include "performConnection.h"
#define BUFFR 512
#define MAXGAMENAME 50

/**
 * strcat Funktion, die den String in einem neuen temporären String speichert,
 * um angepasste Strings zur korrekten Übertragung zu übergeben
 */

int strcattotemp(char* dest, char* src, char* temp){
	strcpy(temp, src);
	strcat(temp, dest);
	return EXIT_SUCCESS;
}

// Formatierten String an Server senden
void sendReplyFormatted(int sock, char* reply) {
	char* container;
	container = malloc(sizeof(char) * (strlen(reply) + 2));
	strcpy(container, reply);
	strcat(container, "\n");
	send(sock, container, strlen(container), 0);
	free(container);
}
 
// Funktion überprüft Nachricht vom Server auf Vollständigkeit
char* checkMessage(int sock, char* message) {
	int i;
	char* bufMes = malloc(sizeof(char) * BUFFR);

	if(message[strlen(message)-1] != '\n')
	{
		i = recv(sock, bufMes, BUFFR, 0);
		if(i == 0)
			perror("\nFehler beim Datenempfang\n");
		strcat(message, bufMes);
	}
	free(bufMes);
	return message;
}

/*
 *Funktion um Servernachricht zeilenweise auszugeben. Gibt die letzte Zeile als
 *Ergebnis zurück. 
 */
char* readLines(char* line) {
	char del[] = "\n";
	char* comp = malloc(sizeof(char) * BUFFR);
	char* ptr;
	
	strcpy(comp, line);	
	
	ptr = strtok(comp, del);
	while(ptr != NULL) {
		//printf("\n%s\n", ptr);
		strcpy(line, ptr);;
		// naechsten Abschnitt erstellen
 		ptr = strtok(NULL, del);
	}
	free(comp);
	free(ptr);
	return line;
}

/*
 *Funktion überprüft ob String == "+ WAIT". Gibt 1 zurück wenn ja, 0 wenn nicht.
 */
int checkWait(char* check) {
	char waitToken[] = "+ WAIT";

	if(strcmp(check,waitToken) == 0) {
		printf("Du hast es geschafft.\n");
		return 1;
	}
	else
	{
		return 0;
	}
}

/* Protokollphase Prolog implementieren */

// Übergabe von host und ip als Parameter eigentlich nicht nötig, evtl. streichen?
int performConnection(int sock, char* version, char* game_id, int fd[]){

	/**
	* Variablen zum Zwischenspeichern und Behandeln der Daten
	* der Kommunikation
	*/
	
	int err;
	char* reader = malloc(sizeof(char) * BUFFR);
	char* temp = malloc(sizeof(char) * BUFFR);
	char* buffer = malloc(sizeof(char) * BUFFR);
	char* player = malloc(sizeof(char) * 8);

	/*Variablen für Schleife*/
	int exitFlag = 1;
	int count = 0;

	while(exitFlag == 1)
	{	
		count++;
		err = recv(sock, buffer, BUFFR, 0);

		//Servernachricht komplett empfangen
		strcpy(reader,checkMessage(sock, buffer));
		if(buffer[0] == '-')
		{
			printf("Fehler: %s\n", reader);
			return EXIT_FAILURE;
		}
		if(err == 0)
		{
			perror("\nFehler beim Datenempfang\n");
			return EXIT_FAILURE;
		}
		
		//Servernachricht zeilenweise ausgeben
		reader = readLines(reader);
		//Auf "+ WAIT" überprüfen
		if(checkWait(reader) == 1) {
			printf("%s\n", reader);
			count = 5;
		}
		
		switch(count)
		{
			case 1:
				strcattotemp(version, "VERSION ", temp);
				sendReplyFormatted(sock, temp);
				break;
			case 2:
				strcattotemp(game_id, "ID ", temp);
				sendReplyFormatted(sock, temp);
				break;
			case 3: 
				strcattotemp(player, "PLAYER ", temp);
				sendReplyFormatted(sock, temp);
				break;
			case 4:
				printf("Case 4 wurde durchlaufen.\n");
				/*Da das recv vom Server etwas unberechenbar ist, steht im Case 4 nichts drin.*/
				break;
			case 5:
				printf("Ende Prolog!\n");
				exitFlag = 0;
				break;
			default:
				perror("\nDefault!\n");
				break;
		}
		//Speicher überschreiben um Fehler zu vermeiden
		memset(reader, '\0', strlen(reader));
		memset(buffer, '\0', strlen(buffer));
	}
	free(reader);
	free(temp);
	free(buffer);
	free(player);
	return EXIT_SUCCESS;
}

