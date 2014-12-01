#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
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
#define VERSION "1.0"



int main(int argc, char *argv[]) {

  // Game-ID als Kommandozeilenparameter auslesen und überprüfen
  char game_id[12];

  if (argc < 2){
    perror("\nGame-ID nicht eingegeben\n");
    return EXIT_FAILURE;
}
  

  strcpy(game_id, argv[1]);

  if (strlen(game_id) != 11) {
    perror("\nGame-ID nicht 11-stellig\n");
    return EXIT_FAILURE;
  }

  // Pipe erstellen
  int fd[2];

  /* Verbindung zu Gameserver aufbauen */

  // Variablen für Socket und Host deklarieren
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in host;

  // Hostname in IP Adresse übersetzen
  struct hostent* ip = gethostbyname(HOSTNAME);
  if (ip == NULL) {
    perror("\nFehler beim Anfordern der IP.\n");
    return EXIT_FAILURE;
  }

  // Benötigte Variablen der Struktur Host Werte zuweisen
  memcpy(&(host.sin_addr), ip->h_addr_list[0], ip->h_length);
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
    perror("\nSocket geschlossen.\n");
    return EXIT_FAILURE;
  }

  close(sock);
  perror("\nSocket geschlossen.\n");
  return EXIT_SUCCESS;
}
