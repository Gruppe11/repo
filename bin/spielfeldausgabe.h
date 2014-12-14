#ifndef spielfeldausgabe
#define spielfeldausgabe

void spielfeldleeren(spielfeld * spielfeld);
int getzahl(char Buchstabe);
void steinespeichern(int spielernummer,char * position,spielfeld * spielfeld);
void zwischenzeile(int anzahlstriche);
void zeichenform(int zeichen, char pos);
void printspielfeld(spielfeld * spielfeld);

#endif
