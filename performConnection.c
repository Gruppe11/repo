#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <winsock2.h>
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



/* Protokollphase Prolog implementieren */

// Übergabe von host und ip als Parameter eigentlich nicht nötig, evtl. streichen?
int performConnection(int sock, char* version, char* game_id, int fd[]){

  /**
   * Variablen zum Zwischenspeichern und Behandeln der Daten
   * der Kommunikation
   */
  int err;
  char* reader = malloc(sizeof(char) * 20);
  char* temp = malloc(sizeof(char) * BUFFR);
  char* buffer = malloc(sizeof(char) * BUFFR);

  // Vom Server geschickte Client-Version einlesen und überprüfen
  err = recv(sock, buffer, BUFFR - 1, 0);
  sscanf(buffer, "%*s%*s%*s%s", reader);
  if (err > 0) {
    buffer[err] = '\0';
  }

  printf("\nDie Version des Servers ist: %s.\n", reader);
  // Mit eigener, formatierter Version antworten
  strcattotemp(version, "VERSION ", temp);
  sendReplyFormatted(sock, temp);

  // Reaktion des Servers auf Version einlesen und verarbeiten
  err = recv(sock, buffer, BUFFR - 1, 0);
  if (err > 0) {
    buffer[err] = '\n';
  }
  if (buffer[0] == '-') {
    printf("\nDie Version %s des Clients wird vom Server nicht akzeptiert!\n", version); // welche Version?
    return EXIT_FAILURE;
  }else {
    printf("\nDie Client-Version wurde akzeptiert.\n");
  }

  // Formatierte Game-ID an Server schicken, um Spiel beizutreten
  strcattotemp(game_id, "ID ", temp);
  sendReplyFormatted(sock, temp);

  // Serverantwort mit Gamekind-Name auslesen und überprüfen
  err = recv(sock, buffer, BUFFR - 1, 0);
  if (err > 0){
    buffer[err] = '\0';
  }
  sscanf(buffer, "%*s%*s%s", reader);

  if(buffer[0] == '-') {
    printf("\nFalsche Game-ID.\n");
    return EXIT_FAILURE;
  }else if (strcmp(reader, "NMMorris") != 0) {
    printf("\nFalsches Server-Spiel.\n");
    return EXIT_FAILURE;
  }else {
    printf("\nSpiel passt.\n");
  }

  // Game-Name von Server einlesen und überprüfen
  // Folgende Variablen an Funktioneanfang??
  char game_name[MAXGAMENAME];
  char* playernumber = "4"; // 4 ist hier einfach beliebig gewählt
  int player_count;
  err = recv(sock, buffer, BUFFR - 1, 0);
  if(err > 0) {
    buffer[err] = '\0';
  }
  sscanf(buffer, "%s", game_name);
  printf("\nSpiel: %s\n", game_name);

  // Gewünschte Spielernummer formatiert an Server senden
  strcattotemp(playernumber, "PLAYER ", temp);
  sendReplyFormatted(sock, temp);

  // Serverantwort Spielernummer einlesen und überprüfen
  err = recv(sock, buffer, BUFFR - 1, 0);
  if (err > 0) {
    buffer[err] = '\0';
  }

  if (buffer[0] == '-') {
    printf("Fehler bei Spielernummer.\n");
    return EXIT_FAILURE;
  } else {
    printf("Spielernummer %s ist akzeptiert.\n", playernumber);
  }

  // Spielername Gegner von Server auslesen und überprüfen
  err = recv(sock, buffer, BUFFR - 1, 0);
  if (err > 0) {
    buffer[err] = '\0';
  }
  if (buffer[0] == '-') {
    printf("\nFehler bei Übermittlung von Spieldaten.\n");
    return EXIT_FAILURE;
  }

  // Spieleranzahl überprüfen
  err = recv(sock, buffer, BUFFR - 1, 0);
  sscanf(buffer, "%i", &player_count);
  if (player_count != '2') {
    printf("\nSpieleranzahl fehlerhaft.\n");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}