#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include "client.h"

char userName[NAME_SIZE+1];
char chatMatrix[CHAT_ROWS][CHAT_COLS] = {'\0'};
int rowPointer = 1;

int
main(void) {
	int fd;
	int amount[1];
	int* pids;
	if((fd = open("server.cfg", O_RDONLY)) < 0) {
		printf("Server is offline. Try connecting again later...\n");
		exit(1);
	} else {
		int rd;
		read(fd, amount, (size_t)sizeof(int));
		pids = malloc(amount[0]*sizeof(int));
		if((rd = read(fd, pids, (size_t)amount[0]*sizeof(int))) < \
		(size_t)amount[0]*sizeof(int)) {
			perror("Failed to read pids on server.cfg");
			close(fd);
			exit(3);
		} else {
			printf("Server online...\nSelect a chat room:\n\n");
			int i;
			for(i = 0; i < amount[0]; i++) {
				printf("Room %d - PID: %d\n", i+1, pids[i]);
			}
		}
	}
	char opt[MAX_ROOM_DIGITS+1] = {'\0'};
	int nOpt;
	do {
		printf("\nRoom number: ");
		scanf("%s", opt);
		opt[MAX_ROOM_DIGITS] = '\0';
		nOpt = atoi(opt);
	}while (!isNumber(opt) || nOpt < 1 || nOpt > amount[0]);
	connect();
	printf("Welcome to chat room nbr. %d\nPID: %d\n", nOpt\
	, pids[atoi(opt)-1]);
	welcome(nOpt, pids[nOpt], userName, getpid());
	exit(0);
}

boolean
isNumber(char *s) {
	while(s++ != '\0') {
		if(!isdigit(*s)) {
			return FALSE;
		}
	}
	return TRUE;
}

void
connect() {
	do {
		printf("User name: ");
		scanf("%s", userName);
		while(getchar()!='\n');
		userName[NAME_SIZE] = '\0';
	} while(!validateUserName(userName));
}

boolean
validateUserName(char *userName) {
	return TRUE; //HARDCODEADO!!!
}

void
receiveMessage(char *msg, char *userName) {
	if(rowPointer == CHAT_ROWS) {
		scrollDown();
		rowPointer=CHAT_ROWS-1;
	}
	int i, j;
	
	for(i = 0; i < NAME_SIZE && userName[i] != '\0' ; i++) {
		chatMatrix[rowPointer][i] = userName[i];
	}

	chatMatrix[rowPointer][i++] = ':';
	chatMatrix[rowPointer][i++] = ' ';

	for(j = 0; j < MESSAGE_SIZE && msg[j] != '\0'; j++) {
		
		if(((i+1) % (CHAT_COLS)) == 0) {
			rowPointer++;
			i = 0;
		}
		chatMatrix[rowPointer][i++] = msg[j];
	}
	rowPointer++;
}

void
clearLastRow(void) {
	int i;
	for(i = 0; i < CHAT_COLS ; i++) {
		chatMatrix[CHAT_ROWS-1][i] = '\0';
	}
}

void
welcome(int opt, int roomPid, char* userName, int pid) {
	int i, j;
	char msg[MESSAGE_SIZE+1] = {'\0'};
	for(i = 0; i < CHAT_ROWS ; i++) {
		for(j = 0; j < CHAT_COLS ; j++) {
			printf("%c", chatMatrix[i][j]);
		}
		printf("\n");
	}
	i = 0;
	while(i++ < CHAT_COLS) {
		printf("-");
	}
	printf("\n");
	printf("\n%s: ", userName);
	i = 0;
	char c;
	while((c = getchar()) != '\n' && i < MESSAGE_SIZE -1) {
		msg[i++] = c;
	}
	msg[MESSAGE_SIZE] = '\0';
	sendMessage(msg, userName);
	welcome(opt, roomPid, userName, pid);
}

void
sendMessage(char* msg, char* userName) {
	receiveMessage(msg, userName);//Esto lo haria el server... HARDCODEADO!!!
}

void
scrollDown() {
	int i, j;
	for(i = 0; i < CHAT_ROWS - 1; i++) {
		for(j = 0; j < CHAT_COLS; j++) {
			chatMatrix[i][j] = chatMatrix[i+1][j];
		}
	}
	clearLastRow();
}
