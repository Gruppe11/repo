#ifndef shmstruct
#define shmstruct

// struct für die allgemeinen Infos
typedef struct shm {

	int thinkerpid;
	int connectorpid;
	int anzahlspieler;
	int eigspielernummer;
	char spielname[60]; // wie lang ist der Gamename maximal?

	// struct für die Spielereigenschaften
	struct spieleratt {
		int spielernummer; // wie lang Spielernummer
		char spielername[128];
		int regflag; //flag: 1=ja 0=nein
	} spieleratt[8]; // im Moment für 2 Spieler: spieleratt[2]

}shm;

int initshm(int size);
struct shm* bindshm(int shmID);
int delshm(int shmID);

#endif