#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "config.h"

#define BUFFR 512
#define TOKEN "="


/* Fehlt eventuell Fehlerbehandlung?? */

// Konfigurationsdaten von Konfigurationsdatei in configstruct einlesen
struct config get_config(char *filename) {

	struct config configstruct;
	char* confvalue = malloc(sizeof(char) * BUFFR);
	char* confitem = malloc(sizeof(char) * BUFFR);

	FILE* file = fopen(filename, "r");

	/* Überprüfen auf Fehler beim Öffnen der Datei. Default verwenden, wenn Fehler. */
	if (file == NULL) {
		printf("\nFehler beim Öffnen der Konfigurationsdatei: Verwende Default.\n");
		file = fopen("client.conf", "r");
	}

	while (fscanf(file, "%s = %s", confitem, confvalue) == 2) {

		if (strcmp(confitem, "hostname") == 0) {
			strncpy(configstruct.hostname, confvalue, BUFFR);
		}
		if (strcmp(confitem, "portnumber") == 0) {
			configstruct.portnumber = atoi(confvalue);
		}
		if (strcmp(confitem, "gamekindname") == 0) {
			strncpy(configstruct.gamekindname, confvalue, BUFFR);
		}

	}

	free(confvalue);
	free(confitem);

	fclose(file);
	return configstruct;

}