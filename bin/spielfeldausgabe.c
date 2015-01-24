#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <netdb.h>
#include <netinet/in.h>
#include "sharedmemory.h"
#include "spielfeldausgabe.h"

/* Steine speichern und Spielfeldausgabe  */



void spielfeldleeren(Spielfeldshm * spielfeld){
	int i;
	int j;
	
	for(i=0; i<3;i++){
		
		for(j=0; j<8;j++){
		
			spielfeld->feld[i][j]= -1;
		
		}
	
	}


}
int getzahl(char Buchstabe){

	switch(Buchstabe) {
		case 'A': return 0; 
		case 'B': return 1; 
		default: return 2; 
		
	}


}

void steinespeichern(int spielernummer, char * position, Spielfeldshm * spielfeld){
	
	
	if(strlen(position)>1){
		char string;
		int zahl;
		
		sscanf(position, "%c%d",&string,&zahl);
		int Buchstaben = getzahl(string);
		spielfeld->feld[Buchstaben][zahl] = spielernummer;

	}

}

void zwischenzeile(int anzahlstriche){

	switch(anzahlstriche) {
		
		case 3: printf("  |           |           |\n"); break;
		case 5: printf("  |   |       |       |   |\n"); break;
		case 6: printf("  |   |   |       |   |   |\n"); break;

	}

}

void zeichenform(int zeichen, char pos){
	
	switch(pos) {
		
		case 'G':
				if(zeichen == -1){ 
					printf("  +-"); break;
				 }
				else if(zeichen == 0){
					printf(" (#)");break;
				 }
				else{
					printf(" (*)");break;
				 }
		case 'L':
				if(zeichen == -1){ 
					printf(" +-"); break;
				 }
				else if(zeichen == 0){
					printf("(#)");break;
				 }
				else{
					printf("(*)");break;
				 }
		
		case 'M':
				 if(zeichen == -1){ 
					printf("-+-"); break;
				 }
				else if(zeichen == 0){
					printf("(#)");break;
				 }
				else{
					printf("(*)");break;
				 }
		case 'R': 
				 if(zeichen == -1){ 
					printf("-+ "); break;
				 }
				else if(zeichen == 0){
					printf("(#)");break;
				 }
				else{
					printf("(*)");break;
				 }
		case 'T': 
				 if(zeichen == -1){ 
					printf("-+ "); break;
				 }
				else if(zeichen == 0){
					printf("(#)");break;
				 }
				else{
					printf("(*)");break;
				 }
	}

}
void printspielfeld(Spielfeldshm * spielfeld){

// erste Zeile
zeichenform(spielfeld->feld[0][0],'G');
printf("---------");
zeichenform(spielfeld->feld[0][1],'M');
printf("---------");
zeichenform(spielfeld->feld[0][2],'R');
printf("\n");

zwischenzeile(3);

// zweite zeile
printf("  |  ");
zeichenform(spielfeld->feld[1][0],'L');
printf("-----");
zeichenform(spielfeld->feld[1][1],'M');
printf("-----");
zeichenform(spielfeld->feld[1][2],'R');
printf("  |\n");


zwischenzeile(5);

// dritte zeile
printf("  |   |  ");
zeichenform(spielfeld->feld[2][0],'L');
printf("-");
zeichenform(spielfeld->feld[2][1],'M');
printf("-");
zeichenform(spielfeld->feld[2][2],'R');
printf("  |   |\n");

zwischenzeile(6);

// vierte zeile
zeichenform(spielfeld->feld[0][7],'G');
printf("-");
zeichenform(spielfeld->feld[1][7],'M');
printf("-");
zeichenform(spielfeld->feld[2][7],'T');
printf("     ");
zeichenform(spielfeld->feld[2][3],'L');
printf("-");
zeichenform(spielfeld->feld[1][3],'M');
printf("-");
zeichenform(spielfeld->feld[0][3],'R');
printf("\n");

zwischenzeile(6);

// fünfte zeile
printf("  |   |  ");
zeichenform(spielfeld->feld[2][6],'L');
printf("-");
zeichenform(spielfeld->feld[2][5],'M');
printf("-");
zeichenform(spielfeld->feld[2][4],'T');
printf("  |   |\n");

zwischenzeile(5);

// sechste zeile
printf("  |  ");
zeichenform(spielfeld->feld[1][6],'L');
printf("-----");
zeichenform(spielfeld->feld[1][5],'M');
printf("-----");
zeichenform(spielfeld->feld[1][4],'T');
printf("  |\n");

zwischenzeile(3);

// siebte zeile
zeichenform(spielfeld->feld[0][6],'G');
printf("---------");
zeichenform(spielfeld->feld[0][5],'M');
printf("---------");
zeichenform(spielfeld->feld[0][4],'R');
printf("\n");






}
