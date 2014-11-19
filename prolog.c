#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <winsock2.h>
//#include <netdb.h>
//#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include "performConnection.h"


/**
 * Implementierung der ersten Protokollphase der
 * Kommunikation mit dem Gameserver
 *
 */


// Zur Verbindung nötige Konstanten deklarieren
// String-Konstanten evtl. als const char hostname[] deklarieren?
#define GAMEKINDNAME "NMMOrris"
#define PORTNUMBER 1357
#define HOSTNAME "sysprak.priv.lab.nm.ifi.lmu.de"
#define VERSION 1.42 //Welche Version hat der Server? Kriege keine Verbindung.



int main(int argc, char *argv[]) {

  // Game-ID als Kommandozeilenparameter auslesen und überprüfen
  char game_id[12];

  if (argc < 2) {
    printf("Game-ID nicht eingegeben.\n");
  }

  strcpy(game_id, argv[1]);

  if (strlen(game_id) != 11) {
    printf("Game-ID nicht 11-stellig.\n");
  }

  // Pipe erstellen und überprüfen
  int fd[2];

  if (pipe(fd) < 0) {
    perror("\nFehler beim Erstellen der Pipe.\n");
    return EXIT_FAILURE;
  }



  /* Verbindung zu Gameserver aufbauen */

  // Variablen für Socket und Host deklarieren
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in host;

  // Hostname in IP Adresse übersetzen
  struct hostent* ip = gethostbyname(HOSTNAME);
  if (ip == NULL) {
    perror("\nFehler beim Anfordern der IP.\n");
  }

  // Benötigte Variablen der Struktur Host Werte zuweisen
  memcpy(&(host.sin_addr), ip->h_addr, ip->h_length);
  host.sin_family = AF_INET;
  host.sin_port = htons(PORTNUMBER);

  // Verbindung aufbauen und überprüfen
  if (connect(sock, (struct sockaddr*) &host, sizeof(host)) == 0) {
    printf("\nVerbindung hergestellt!\n");
  }else{
    perror("\nFehler beim Verbindungsaufbau.\n");
    return EXIT_FAILURE;
  }

  // Prologphase der Kommunikation mit dem Server durchführen und testen
  if (performConnection(sock, VERSION, game_id, fd) != 0) {
    close(sock);
    return EXIT_FAILURE;
  }

  close(sock);
  return EXIT_SUCCESS;
}