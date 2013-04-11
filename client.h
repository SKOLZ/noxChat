#include "server.h"

#ifndef CLIENT_H
#define CLIENT_H
    void connect();
	boolean validateUserName(char *userName, int room);
    boolean isNumber(char *s);
    void welcome(int opt);
	void printDivision(void);
    boolean isValidRoomNumber(char *opt, int rooms);
	boolean checkUserInServer(char *userName, int room, pid_t pid);
	void askRoomNumber(int rooms);
    void waitForMessages(void);
    void prompt(void);
#endif
