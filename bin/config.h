#ifndef cnfig
#define cnfig

#define BUFFR 512

// char[] Datentyp für portnumber sollte laut Aufgabenstellung anders sein
struct config {
	char hostname[BUFFR];
	char portnumber[BUFFR];
	char gamekindname[BUFFR];
};

struct config configstruct;
struct config get_config(char *filename);

#endif