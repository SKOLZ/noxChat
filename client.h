#ifndef CLIENT_H
#define CLIENT_H
	#include "server.h"

	void connect();
	boolean isValidUserName(char *userName);
	boolean isNumber(char *s);
	void welcome(int opt, int roomPid, char *userName, int pid);
	void receiveMessage(char *msg, char *userName);
	void scrollDown();
	void sendMessage(char *msg, char *userName);
	void printDivision(void);
    void checkRowPosition(void);
    boolean isValidRoomNumber(char *opt, int rooms);
#endif
