#ifndef cnfig
#define cnfig

#define BUFFR 512


struct config {
	char hostname[BUFFR];
	int portnumber;
	char gamekindname[BUFFR];
};

struct config configstruct;
struct config get_config(char *filename);

#endif
