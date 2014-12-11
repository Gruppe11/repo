#ifndef shmstruct
#define shmstruct

// struct für die allgemeinen Infos
typedef struct {

	int thinkerpid;
	int connectorpid;
	int anzahlspieler;
	int eigspielernummer;
	char spielname[60]; // wie lang ist der Gamename maximal?

	// struct für die Spielereigenschaften
	struct spieleratt {
		int spielernummer; // wie lang Spielernummer
		char spielername[128];
		int regflag; // flag: 1=ja 0=nein
	} spieleratt[8]; // im Moment für 2 Spieler: spieleratt[2]

} shm;

//struct für spielfeld
typedef struct {
	
	int anzsteine;
	int feld[3][8];
	
} spielfeld;

int initshm(int size);
void bindshm(int shmID, shm* shmpointer);
void bindfeld(int feldID, spielfeld* shmpointer);
int delshm(int ID);

#endif