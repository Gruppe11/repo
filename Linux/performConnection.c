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
 * String an Server senden
 *
 * sock: Socket des Servers
 * serverMessage: Pointer auf String, in dem die zu übermittelnde Nachricht gespeichert ist
 */
void sendMessage(int sock, char* clientMessage) {
	int num = send(sock, clientMessage, strlen(clientMessage), 0);
	if (num < 0) {
		perror("ERROR writing to socket");
		exit(1);
	}
}

/**
 * Liest eine vollständige Nachricht (letztes Zeichen = '\n') von Server.
 *
 * sock: Socket des Servers
 * serverMessage: Pointer auf String, in den die Nachricht geschrieben werden soll
 */
void getMessage(int sock, char* serverMessage) {
	char buffer[BUFFR];
	int numToken;
	int numTries = 10;

	bzero(serverMessage, BUFFR); //Fülle String mit Nullen

	do {
		bzero(buffer, BUFFR); //Fülle String mit Nullen
		numToken = recv(sock, buffer, BUFFR, 0);

			if (numToken < 0)
				perror("ERROR reading from socket");

		strcat(serverMessage, buffer);

		numTries--;
	} while ((buffer[numToken-1] != '\n') && numTries > 0);
}

/**
 * Kommunikation mit dem Server
 *
 * sock: Socket des Servers
 * version: Version des Clients
 * game_id: Game ID des Spiels
 * fd: Pipe für ???
 */
int performConnection(int sock, char* version, char* game_id, int fd[]) {

	char serverMessage[BUFFR];
	char clientMessage[BUFFR];
	char* line = NULL;
	int numTries = 10; //Anzahl der Versuche eine Nachricht vom Server zu erhalten
	char* errorMessage;

	while(1) {
		bzero(serverMessage, BUFFR); //Fülle String mit Nullen
		getMessage(sock, serverMessage); //Empfange Nachricht von Server

		//Servernachricht verarbeiten
		line = strtok(serverMessage, "\n");
		while (line != NULL) {

			//Switch zwischen positiver/negativer Server Antwort
			switch (line[0]) {

				//positive Antwort
				case '+':

					//gebe Servernachricht aus
					printf("S:%s\n", line);

					if (strstr(line, "WAIT") != 0) {
						//sende OKWAIT
						sprintf(clientMessage, "OKWAIT\n");
						sendMessage(sock, clientMessage);
						printf("C: %s", clientMessage);
					} else if (strstr(line, "MNM Gameserver") != 0) {
						//sende  Protocol Version
						sprintf(clientMessage, "%s %s\n", "VERSION", version);
						sendMessage(sock, clientMessage);
						printf("C: %s", clientMessage);
					} else if (strstr(line, "Client version accepted") != 0) {
						//sende Game ID
						sprintf(clientMessage, "%s %s\n", "ID", game_id);
						sendMessage(sock, clientMessage);
						printf("C: %s", clientMessage);
					} else if (strstr(line, "PLAYING") != 0) {
						//sende Player
						sprintf(clientMessage, "%s\n", "PLAYER");
				    	sendMessage(sock, clientMessage);
				    	printf("C: %s", clientMessage);
					}
					break;

				//negative Antwort - Error Handling
				case '-':

					//gebe Servernachricht aus
					errorMessage = strndup(line+1, strlen(line)-1);
					printf("ERROR:%s\n", errorMessage);

					exit(1);
			}

			line = strtok(NULL, "\n");
			numTries = 10; //Anzahl der Versuche resetten
		}

		if (numTries == 0) {
			printf("ERROR receiving answer from server\n");
			exit(1);
		}

		numTries--;
	}

	return 0;
}