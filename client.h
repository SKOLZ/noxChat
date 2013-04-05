#ifndef CLIENT_H
#define CLIENT_H

	void connect();
	boolean validateUserName(char *userName);
	boolean isNumber(char *s);
	void welcome(int opt, int roomPid, char *userName, int pid);
	void receiveMessage(char *msg, char *userName);
	void scrollDown();
	void sendMessage(char *msg, char *userName);
	void printDivision(void);
	boolean checkUserInServer(char *userName, int room);
#endif
