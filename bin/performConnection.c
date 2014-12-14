
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <netdb.h>
#include <netinet/in.h>
#include "sharedmemory.h"
#include "performConnection.h"
#include "config.h"
#include "spielfeldausgabe.h"
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
		perror("\nError beim Schreiben zum Socket");
		exit(EXIT_FAILURE);
	}

}

/**
 * Liest eine vollständige Nachricht (letztes Zeichen = '\n') von Server.
 *
 * sock: Socket des Servers
 * serverMessage: Pointer auf String, in den die Nachricht geschrieben werden soll
 */
void getLine(int sock, char* line2) {

	static char* line;
	static char serverMessage[BUFFR];
	char buffer[BUFFR];
	int numToken;
	int numTries = 10;

	line = strtok(NULL, "\n"); // nächste Zeile

	if (line == NULL)
	{
		bzero(serverMessage, BUFFR); // Fülle String mit Nullen

		do {
			bzero(buffer, BUFFR); // Fülle String mit Nullen
			numToken = recv(sock, buffer, BUFFR, 0);

				if (numToken < 0) {
					perror("\nError beim Lesen vom Socket");
				}

			strcat(serverMessage, buffer);

			numTries--;
		} while ((buffer[numToken-1] != '\n') && numTries > 0);

		line = strtok(serverMessage, "\n");
	}

	strcpy(line2, line);
}

int getSock(struct config configS) {

	struct sockaddr_in host;
	int sock;
	
	// Hostname in IP Adresse übersetzen
	struct hostent* ip = gethostbyname(configS.hostname);
	//printf("IP: %s\n", ip->h_name);
	if (ip == NULL) {
		fprintf(stderr, "\nFehler beim Anfordern der IP\n");
		return EXIT_FAILURE;
	}

	// Benötigte Variablen der Struktur Host Werte zuweisen
	memcpy(&(host.sin_addr), ip->h_addr_list[0], ip->h_length);
	host.sin_family = AF_INET;
	host.sin_port = htons(configS.portnumber);
	//printf("SIN_PORT: %i\n", host.sin_port);

	// Verbindung aufbauen und überprüfen
	sock = socket(AF_INET, SOCK_STREAM, 0);
	//printf("SOCKET: %i\n", sock);
	if (connect(sock, (struct sockaddr*) &host, sizeof(host)) == 0) {
		printf("\nVerbindung hergestellt!\n");
	} else {
		perror("\nFehler beim Verbindungsaufbau");
		return EXIT_FAILURE;
	}
	return sock;
}

/**
 * Kommunikation mit dem Server
 *
 * sock: Socket des Servers
 * version: Version des Clients
 * game_id: Game ID des Spiels
 * fd: Pipe für ???
 */
int performConnection(char* version, char* game_id, int fd[],shm * shm) {

	char* line = malloc(sizeof(char) * BUFFR);
	char clientMessage[BUFFR];
	char* errorMessage;
	int sock;

	// Temporärer Speicher für das Auslesen von Variablen aus den Serverantworten
	char* temp1 = malloc(sizeof(char) * 20);
	int temp2;
	int temp3;

	// Flags zum eindeutigen Kommunikation miit Server
	int phase = 0; // Flag zur Bestimmung der Phase
	int capture = 0; // Flag zum Capture Wert
	//flag um zu wissen, ob feldshm schon erstellt wurde
	int shmflag = 0;
	spielfeld *spielfeld = malloc(sizeof(spielfeld));



	sock = getSock(configstruct);

	while (1) {
		getLine(sock, line); // Empfange Nachricht von Server

		// gebe Servernachricht aus (für Debugging)
		//printf("	S:%s\n", line);

		// Servernachricht verarbeiten
		// Switch zwischen positiver/negativer Server Antwort
		switch (line[0]) {

			// positive Antwort
			case '+':

				if (phase == 0) {

					/* Protokollphase Prolog */

					if (strstr(line, "+ MNM Gameserver") != 0) {

						// sende  Protocol Version
						sprintf(clientMessage, "%s %s\n", "VERSION", version);
						sendMessage(sock, clientMessage);

						// lese Serverversion ein + gebe diese aus
						sscanf(line, "%*s %*s %*s v%s", temp1);
						printf("\nServer Version %s\n", temp1);

					} else if (strstr(line, "+ Client version accepted") != 0) {

						// sende Game ID
						sprintf(clientMessage, "%s %s\n", "ID", game_id);
						sendMessage(sock, clientMessage);

						// gebe Info aus
						printf("Client Version %s akzeptiert\n", version);

					} else if (strstr(line, "+ PLAYING") != 0) {

						// sende Player
						sprintf(clientMessage, "%s\n", "PLAYER");
						sendMessage(sock, clientMessage);

						// lese Gametyp ein + kontrolliere diesen
						sscanf(line, "%*s %*s %s", temp1);
						if (strstr(temp1, "NMMorris") == 0) {
							perror("\nFalscher Spieltyp");
							return EXIT_FAILURE;
						}
						printf("Spieltyp %s akzeptiert\n\n", temp1);

						getLine(sock, line); // nächste Zeile

						// lese Spielname ein + gebe diesen aus + speichern in shm
						sscanf(line, "%*s %[^\n]s", temp1);
						printf("Spielname: %s\n", temp1);
						strncpy(shm->spielname, temp1, sizeof(shm->spielname));

					} else if (strstr(line, "+ YOU") != 0) {

						// lese eigene Spielervariablen ein + gebe diese aus + speichern in shm
						sscanf(line, "%*s %*s %d %[^\n]s", &temp2, temp1);
						printf("Du (%s) bist Spieler #%d\n", temp1, temp2 + 1); // Spielernummer nur für die Ausgabe +1 -> Nummern 1-8
						shm->eigspielernummer = temp2;
						
						if (shm->eigspielernummer < 0) {
							
							fprintf(stderr, "\nFehler beim Erfassen der Spielernummer: Spielernummer < 0\n");
							return EXIT_FAILURE;
						
						}
						
						shm->spieleratt[shm->eigspielernummer].spielernummer = shm->eigspielernummer;	
						strncpy(shm->spieleratt[shm->eigspielernummer].spielername, temp1, sizeof(shm->spieleratt[shm->eigspielernummer].spielername));	
						shm->spieleratt[shm->eigspielernummer].regflag = 1;

						getLine(sock, line); // nächste Zeile

						// lese Anzahl Spieler aus + speichern in shm
						sscanf(line, "%*s %*s %d", &temp2);
						shm->anzahlspieler = temp2;
						
						if (shm->anzahlspieler < 1) {
							
							fprintf(stderr, "\nFehler beim Erfassen der Spieleranzahl: Spieleranzahl < 1\n");
							return EXIT_FAILURE;
						
						}						
						else if (shm->anzahlspieler > 8) {
						
							fprintf(stderr, "\nFehler beim Erfassen der Spieleranzahl: Spieleranzahl > 8\n");
							return EXIT_FAILURE;
						
						}
						getLine(sock, line); // nächste Zeile

						// lese Gegner-Spielervariablen ein + gebe diese aus + speichern in shm
						sscanf(line, "%*s %d", &temp2);
						temp1 = strndup(line + 4, strlen(line) - 6);
						temp3 = atoi(strndup(line + 5 + strlen(temp1), 1));
						
						if (temp3 == 1)
							printf("Spieler #%d (%s) ist bereit\n\n", temp2 + 1, temp1); // Spielernummer nur für die Ausgabe +1 -> Nummern 1-8
						else
							printf("Spieler #%d (%s) ist noch nicht bereit\n\n", temp2 + 1, temp1); // Spielernummer nur für die Ausgabe +1 -> Nummern 1-8
						
						shm->spieleratt[temp2].spielernummer = temp2;	
						strncpy(shm->spieleratt[temp2].spielername, temp1, sizeof(shm->spieleratt[temp2].spielername));	
						shm->spieleratt[temp2].regflag = temp3;
					
					} else if (strstr(line, "+ ENDPLAYERS") != 0) {

						// setze Flag für Spielverlaufsphase
						phase = 1;

					}

				} else if (phase == 1) {

					/* Spielverlaufsphase */

					if (strstr(line, "+ WAIT") != 0) {

						// sende "OKWAIT"
						sprintf(clientMessage, "OKWAIT\n");
						sendMessage(sock, clientMessage);
						printf("Warte auf Gegenspieler...\n");

					} else if (strstr(line, "+ MOVE ") != 0) {

						// lese Zeit für Spielzug
						sscanf(line, "%*s %*s %d", &temp2);
						// temp2: Zeit für Spielzug

					} else if (strstr(line, "+ CAPTURE") != 0) {

						// setze Capture Flag
						sscanf(line, "%*s %*s %d", &temp2);
						capture = temp2;
					
						if (shmflag == 0) {

							// Spielfeld shm anlegen
							int feldID;
							int shmSizefeld = sizeof(spielfeld);

							feldID = initshm(shmSizefeld);

							if (feldID < 1) {

								printf("No feld SHM\n");		
								return EXIT_FAILURE;

							}

							// SHM binden 
							bindfeld(feldID, spielfeld);

							// Im Fehler shm -1 
							if (spielfeld == (void *) -1) {

								printf("Fehler binden feld shm\n");
								return EXIT_FAILURE;

							}

							if (delshm(feldID) == -1) {

								fprintf(stderr, "\nFehler bei Zerstoerung von feldshm\n");

							}

							shmflag = 1;
							
						
						}
						spielfeldleeren(spielfeld);
						spielfeld->zuschlagendesteine = temp2;
				
					} else if (strstr(line, "+ PIECELIST") != 0) {

						// lese Anzahl Spieler/Steine pro Spieler
						sscanf(line, "%*s %*s %d,%d", &temp2, &temp3);
						// temp2: Anzahl Spieler
						// temp3: Anzahl Steine pro Spieler

						
						
						spielfeld->anzsteine = temp3;

					} else if (strstr(line, "+ PIECE") != 0) {

						// lese Positionen der Steine
						sscanf(line, "%*s PIECE%d.%d %s", &temp2, &temp3, temp1);
						// temp2: Spielernummer
						// temp3: Steinnummer
						// temp1: Position des Steins
						steinespeichern(temp2, temp1,spielfeld);

					} else if (strstr(line, "+ ENDPIECELIST") != 0) {

						// sende "THINKING"
						sprintf(clientMessage, "THINKING\n");
						sendMessage(sock, clientMessage);
						
						printspielfeld(spielfeld);
					
					} else if (strstr(line, "+ OKTHINK") != 0) {

						// Übergebe Information an Thinker
						// Thinker

						// solange noch kein Thinker aktiv: lese Spielzugeingabe und sende diese formatiert an den Server
						if (capture == 0) {

							printf("Setze einen Stein: ");

						} else if (capture == 1) {

							printf("Entferne einen Stein des Gegners: ");

						}

						scanf("%s", temp1);
						sprintf(clientMessage, "PLAY %s\n", temp1);

						// sende Spielzug
						sendMessage(sock, clientMessage);

					} else if (strstr(line, "+ MOVEOK") != 0) {

						// Spielzug akzeptiert
						printf("Spielzug akzeptiert\n\n");

					} else if (strstr(line, "+ GAMEOVER") != 0) {

						// evtl. Flag setzen für Ende, da Vergleich "+ ENDPIECELIST" sonst nicht eindeutig

					}

				}

				break;

			// negative Antwort - Error Handling
			case '-':

				// gebe Servernachricht aus
				errorMessage = strndup(line + 2, strlen(line) - 2);

				if (strncmp(line, "- exiting", 9) == 0) {
					fprintf(stderr, "\nGame ID nicht gefunden\n");
				} else if (strstr(line, "No free computer player found for that game - exiting") != 0) {
					fprintf(stderr, "\nKein freier Platz vorhanden\n");
				} else if (strstr(line, "Socket timeout - please be quicker next time") != 0) {
					fprintf(stderr, "\nSocket timeout\n");
				} else if (strstr(line, "Protocol mismatch - you probably didn't want to talk to the fabulous gameserver") != 0) {
					fprintf(stderr, "\nProtocol mismatch\n");
				} else if (strstr(line, "We expected you to THINK!") != 0) {
					fprintf(stderr, "\nZuerst OKTHINK senden, erst dann den Spielzug\n");
				} else if (strstr(line, "Destination is already occupied") != 0) {
					fprintf(stderr, "\nSpielzug ungültig: Das Feld ist schon besetzt\n");
				} else if (strstr(line, "You can't capture yourself") != 0) {
					fprintf(stderr, "\nSpielzug ungültig: Du kannst deinen eigenen Stein nicht entfernen\n");
				} else {
					fprintf(stderr, "\nUnerwarteter Serverfehler: %s\n", errorMessage);
				}

				return EXIT_FAILURE;

			// default: ist das überhaupt nötig?
			default:

				fprintf(stderr, "\nServernachricht kann nicht verarbeitet werden\n");
				return EXIT_FAILURE;
			}
	}

	return EXIT_SUCCESS;

}