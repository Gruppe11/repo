#ifndef THINKER
#define THINKER

void SetorDelete(char* nextMove, SharedMem* shm, Spielfeldshm* spielfeld, int availableFlag);
void thinkMove(char* nextMove, SharedMem* shm, Spielfeldshm* spielfeld);
int think(int pipeWrite, SharedMem* shm, Spielfeldshm* spielfeld);

#endif