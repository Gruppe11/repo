#ifndef PerformConnection_h
#define PerformConnection_h

int strcattotemp(char* dest, char* src, char* temp);
void sendReplyFormatted(int sock, char* reply);
int performConnection(int sock, char* VERSION, char* game_id, int fd[]);

#endif
