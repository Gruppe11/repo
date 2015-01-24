#ifndef PerformConnection_h
#define PerformConnection_h

void sendMessage(int sock, char* clientMessage);
void getLine(int sock, char* line2);
int performConnection(char* version, char* game_id, SharedMem *shm, int pipeRead);

#endif