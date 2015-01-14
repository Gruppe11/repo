#ifndef shmstruct
#define shmstruct

// struct für die allgemeinen Infos
typedef struct {

	int think_flag; //Flag ob thinker neuen Spielzug berechnen darf
	int thinkerpid;
	int connectorpid;
	int anzahlspieler;
	int eigspielernummer;
	char spielname[60]; // wie lang ist der Gamename maximal?
	int fieldID;

	// struct für die Spielereigenschaften
	struct spieleratt {
		int spielernummer; // wie lang Spielernummer
		char spielername[128];
		int regflag; // flag: 1=ja 0=nein
	} spieleratt[8]; // im Moment für 2 Spieler: spieleratt[2]

} SharedMem;

//struct für spielfeld
typedef struct {
	int zuschlagendesteine;
	int steineverfuegbar;
	int anzsteine;
	int feld[3][8];
	
} Spielfeldshm;

int initshm(int size);
void* attachshm(int shmID);
int delshm(int ID);

#endif
