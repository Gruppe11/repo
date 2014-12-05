#ifndef PerformConnection_h
#define PerformConnection_h

void sendMessage(int sock, char* clientMessage);
char* getLine(int sock);
int performConnection(int sock, char* version, char* game_id, int fd[],shm * shm);

#endif
