#include "server.h"

void connect();
boolean validateUserName(char *userName);
boolean isNumber(char *s);
void welcome(int opt, int roomPid, char *userName, int pid);
void receiveMessage(char *msg, char *userName);
void scrollDown();
void sendMessage(char *msg, char *userName);
