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
		exit(EXIT_FAILURE);
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
	char* errorMessage;

	//Temporärer Speicher für das Auslesen von Variablen aus den Serverantworten
	char* temp1;
	temp1 = malloc(sizeof(char) * 20);
	int temp2;
	int temp3;

	while(1) {
		getMessage(sock, serverMessage); //Empfange Nachricht von Server

		//Servernachricht verarbeiten
		line = strtok(serverMessage, "\n");
		while (line != NULL) {

			//Switch zwischen positiver/negativer Server Antwort
			switch (line[0]) {

				//positive Antwort
			case '+':

				//gebe Servernachricht aus
//				printf("S:%s\n", line);

/* Wird später für die Spielverlaufsphase gebraucht
				if (strstr(line, "+ WAIT") != 0)
				{
					//sende OKWAIT
					sprintf(clientMessage, "OKWAIT\n");
					sendMessage(sock, clientMessage);
					printf("C: %s", clientMessage);
				}
				else if (strstr(line, "+ ENDPIECELIST") != 0)
				{
					//sende  Protocol Version
					sprintf(clientMessage, "THINKING\n");
					sendMessage(sock, clientMessage);
					printf("C: %s", clientMessage);
				}
*/
				if (strstr(line, "+ MNM Gameserver") != 0) {

					//sende  Protocol Version
					sprintf(clientMessage, "%s %s\n", "VERSION", version);
					sendMessage(sock, clientMessage);

					//lese Serverversion ein + gebe diese aus
					sscanf(line, "%*s %*s %*s v%s", temp1);
					printf("Server Version %s\n", temp1);

				} else if (strstr(line, "+ Client version accepted") != 0) {

					//sende Game ID
					sprintf(clientMessage, "%s %s\n", "ID", game_id);
					sendMessage(sock, clientMessage);

					//gebe Info aus
					printf("Client Version %s akzeptiert\n", version);

				} else if (strstr(line, "+ PLAYING") != 0) {

					//sende Player
					sprintf(clientMessage, "%s\n", "PLAYER");
			    	sendMessage(sock, clientMessage);

					//lese Gametyp ein + kontrolliere diesen
					sscanf(line, "%*s %*s %s", temp1);
					if (strstr(temp1, "NMMorris") == 0) {
						printf("ERROR: Falscher Spieltyp\n");
						return EXIT_FAILURE;
					}
					printf("Spieltyp %s akzeptiert\n\n", temp1);

					//nächste Zeile "+ <<Game-Name>>"
					line = strtok(NULL, "\n");
					if (line == NULL) {
						getMessage(sock, serverMessage);
						line = strtok(serverMessage, "\n");
					}

					//lese Spielname ein + gebe diesen aus
					sscanf(line, "%*s %s", temp1);
					printf("Spielname: %s\n", temp1);

				} else if (strstr(line, "+ YOU") != 0) {

					//lese eigene Spielervariablen ein + gebe diese aus
					sscanf(line, "%*s %*s %d %[^\n]s", &temp2, temp1);
					printf("Du (%s) bist Spieler #%d\n", temp1, temp2 + 1);

					//nächste Zeile "+ TOTAL 2 <<Spieleranzahl>>"
					line = strtok(NULL, "\n");
					if (line == NULL) {
						getMessage(sock, serverMessage);
						line = strtok(serverMessage, "\n");
					}

					//nächste Zeile "+ <<Spielernummer>> <<Spielername>> <<Bereit>>"
					line = strtok(NULL, "\n");
					if (line == NULL) {
						getMessage(sock, serverMessage);
						line = strtok(serverMessage, "\n");
					}

					//lese Gegner Spielervariablen ein + gebe diese aus
					sscanf(line, "%*s %d", &temp2);
					temp1 = strndup(line + 4, strlen(line) - 6);
					temp3 = atoi(strndup(line + 5 + strlen(temp1), 1));
					if (temp3 == 1)
						printf("Spieler #%d (%s) ist bereit\n\n", temp2 + 1, temp1);
					else
						printf("Spieler #%d (%s) ist noch nicht bereit\n\n", temp2 + 1, temp1);

				}

				break;

			//negative Antwort - Error Handling
			case '-':

				//gebe Servernachricht aus
				errorMessage = strndup(line + 2, strlen(line) - 2);

				if (strstr(line, "No free computer player found for that game - exiting") == 0) {
					printf("Kein freier Platz vorhanden\n");
				} else if (strstr(line, "Socket timeout - please be quicker next time") == 0) {
					printf("Socket timeout\n");
				} else if (strstr(line, "Protocol mismatch - you probably didn't want to talk to the fabulous gameserver") == 0) {
					printf("Protocol mismatch\n");
				} else if (strstr(line, "We expected you to THINK!") == 0) {
					printf("Client soll DENKEN\n");
				} else {
					printf("Unerwarteter Serverfehler: %s\n", errorMessage);
				}

				return EXIT_FAILURE;

			//default: ist das überhaupt nötig?
			default:

				printf("ERROR: Servernachricht kann nicht verarbeitet werden\n");
				return EXIT_FAILURE;
			}

			line = strtok(NULL, "\n");
		}
	}

	return EXIT_SUCCESS;
}