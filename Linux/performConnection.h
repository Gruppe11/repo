#ifndef PerformConnection_h
#define PerformConnection_h

void sendMessage(int sock, char* clientMessage);
void getMessage(int sock, char* serverMessage);
int performConnection(int sock, char* version, char* game_id, int fd[]);

#endif
