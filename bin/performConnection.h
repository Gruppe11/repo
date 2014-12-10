#ifndef PerformConnection_h
#define PerformConnection_h

void sendMessage(int sock, char* clientMessage);
void getLine(int sock, char* line);
int performConnection(int sock, char* version, char* game_id, int fd[],shm * shm);

#endif
