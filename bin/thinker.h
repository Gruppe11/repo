#ifndef THINKER
#define THINKER

char *SetorDelete(SharedMem * shm, Spielfeldshm * spielfeld, int availableFlag);
char *thinkMove(SharedMem * shm, Spielfeldshm * spielfeld);
void think(int pipeWrite, SharedMem * shm, Spielfeldshm * spielfeld);

#endif
