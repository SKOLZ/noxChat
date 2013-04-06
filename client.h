#include "server.h"

#ifndef CLIENT_H
#define CLIENT_H
    void connect();
	boolean validateUserName(char *userName, int room);
	boolean isNumber(char *s);
	void welcome(int opt, pid_t roomPid, char *userName, pid_t pid);
	void receiveMessage(char *msg, char *userName);
	void scrollDown();
	void sendMessage(char *msg, char *userName);
	void printDivision(void);
	boolean checkUserInServer(char *userName, int room, pid_t pid);
	void askRoomNumber(int rooms, pid_t *pids);
#endif
