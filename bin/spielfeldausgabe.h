#ifndef spielfeldausgabe
#define spielfeldausgabe

void spielfeldleeren(Spielfeldshm * spielfeld);
int getzahl(char Buchstabe);
void steinespeichern(int spielernummer,char * position, Spielfeldshm * spielfeld);
void zwischenzeile(int anzahlstriche);
void zeichenform(int zeichen, char pos);
void printspielfeld(Spielfeldshm * spielfeld);

#endif
