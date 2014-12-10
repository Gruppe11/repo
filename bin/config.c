#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "config.h"

#define BUFFR 512
#define TOKEN "="


/* Fehlt eventuell Fehlerbehandlung?? */

// String umgebende Leerzeichen entfernen zum Vergleichen
char *trim_space(char *str) {

	char *end;


	// Führende Leerzeichen löschen
	while(isspace(*str)) {
		str++;
	}

	// Alles Leerzeichen
	if(*str == 0) { 
		return str;
	}

	// Nachfolgende Leerzeichen löschen
	end = str + strlen(str) - 1;
	while(end > str && isspace(*end)) {
		end--;
	}
	
	// Neue Nullterminierung hinzufügen
	*(end+1) = 0;

	return str;

}


// String umgebende Leerzeichen entfernen für Ausgabe
size_t trim_space2(char *out, size_t len, const char *str) {

	if (len == 0) {
		return 0;
	}

	const char *end;
	size_t out_size;

	// Führende Leerzeichen löschen
	while (isspace(*str)) {
		str++;
	}
	
	// Alles Leerzeichen?
	if (*str == 0) {
		*out = 0;
		return 1;
	}

	// Nachfolgende Leerzeichen löschen
	end = str + strlen(str) - 1;
	while (end > str && isspace(*end)) {
		end--;
	}
	end++;

	// Ausgabegröße setzen
	out_size = (end - str) < len-1 ? (end - str) : len-1;

	// Getrimmten String kopieren und neue Nullterminierung hinzufügen
	memcpy(out, str, out_size);
	out[out_size] = 0;

	return out_size;

}


// Konfigurationsdaten von Konfigurationsdatei in configstruct einlesen
struct config get_config(char *filename) {

	struct config configstruct;
	FILE *file = fopen(filename, "r");
	
	/* Überprüfen auf Fehler beim Öffnen der Datei. Verwenden Default wenn ja */
	if (file == NULL) {
		printf("\nFehler beim Öffnen der Konfigurationsdatei: Verwende Default.\n");
		file = fopen("client.conf", "r");
	}
	
	char line[BUFFR];
	while(fgets(line, sizeof(line), file) != NULL){
		char *confvalue;
		char *confitem;
		confvalue = strtok(line, TOKEN);
		while (confvalue != NULL) {
			if (confitem == NULL) {
				confitem = trim_space(confvalue);
			} else {
				if (strcmp(confitem, "hostname") == 0) {
					trim_space2(configstruct.hostname, BUFFR, confvalue);
				} else if (strcmp(confitem, "portnumber") == 0) {
					configstruct.portnumber = atoi(confvalue);
				} else if (strcmp(confitem, "gamekindname") == 0) {
					trim_space2(configstruct.gamekindname, BUFFR, confvalue);
				} else {
					// Fehlerbehandlung hier sinnvoll?
					perror("Fehler bei Auslesen der Konfigurationsdatei.\n");
				}
				confitem = NULL;
			}
			confvalue = strtok(NULL, TOKEN);
		}
	}
	

	fclose(file);
	return configstruct;

}
