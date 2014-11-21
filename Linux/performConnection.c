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

/* Schleife statt sequentielle Abfrage */
/* Puffer mit \0 Notation überschreiebn */
/* perror statt printf bei Fehlermeldung */
/* strcpy statt sscanf */

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

  do
	{	
		err = recv(sock, buffer, BUFFR - 1, 0);
		if(err == 0)
		{
				perror("\nFehler beim Datenempfang\n");
				return EXIT_FAILURE;
		}
		printf("%c\n", buffer[err - 1]);
	}while(buffer[err - 1] != '\n');

	printf("%c\n", buffer[0]);
    
  strcpy(reader, buffer);
	printf("Rückgabewert von recv: %i. Komisch oder?\n", err);
	printf("Reader: %s\n", reader);
	
	
	return 0;
}


