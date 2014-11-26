#ifndef shmstruct
#define shmstruct


  // struct für die allgemeinen Infos
struct shm{
   int thinkerpid;
   int connectorpid;
   int anzahlspieler;
   int eigspielernummer;
   // wie lang ist der Gamename maximal?
   char spielname[60];
   // struct für die Spielereigenschaften
   struct spieleratt{
		int spielernummer;
		// wie lang Spielernummer;
		char spielername[128];
		//flag: 1:ja 0:nein
		int regflag;
   // im Moment für 2 Spieler:spieleratt[2]
  }spieleratt[2];
 
 };

int initshm(int size);
struct shm* bindshm(int shmID);
int delshm(int shmID);

#endif
