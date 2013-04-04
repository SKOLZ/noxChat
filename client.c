#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "client.h"

char userName[NAME_SIZE+1];
char chatMatrix[CHAT_ROWS][CHAT_COLS] = {'\0'};
int rowPointer = 0;

int
main(void) {
	system("clear");
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
	askRoomNumber();
	exit(0);
}

void
askRoomNumber(void) {
    char opt[MAX_ROOM_DIGITS+1] = {'\0'};
	int nOpt, i = 0;
    boolean flag;
    char c;
	do {
        i = 0;
        printf("\nRoom number: ");
        flag = TRUE;
        while((c = getchar()) != '\n' && flag) {
            opt[i++] = c;
            if (i == MAX_ROOM_DIGITS) {
                flag = FALSE;
            }
        }
        if(!flag) {
            while(getchar() != '\n');
        }
        opt[MAX_ROOM_DIGITS] = '\0';
		nOpt = atoi(opt);
	}while (!isValidRoomNumber(opt, amount[0]));
    connect();
	welcome(nOpt, pids[nOpt], userName, getpid());
}

boolean
isValidRoomNumber(char *opt, int rooms) {
    int nOpt = atoi(opt);
    
    if (strlen(opt) == 0) {
        printf("ERROR: please introduce a numeric value\n");
        resetOption(opt);
        return FALSE;
    } else if (!isNumber(opt)) {
        printf("ERROR: Introduce a valid number\n");
        resetOption(opt);
        return FALSE;
    } else if(nOpt < 1) {
        printf("ERROR: Invalid option. Option must be greater than zero.\n");
        resetOption(opt);
        return FALSE;
    } else if (nOpt > rooms) {
        printf("ERROR: Invalid option. Option must be >=1 and <=%d.\n", rooms);
        resetOption(opt);
        return FALSE;
    } else {
        return TRUE;
    }
}

void
resetOption(char *opt) {
    int i;
    for (i = 0; i < MAX_ROOM_DIGITS; i++) {
        opt[i] = '\0';
    }
}

boolean
isNumber(char *s) {
	while(*s != '\0') {
		if(!isdigit(*s)) {
			return FALSE;
		}
        s++;
	}
	return TRUE;
}

void
connect(void) {
    int i = 0;
	char c;
    boolean flag;
    do {
        printf("User name: ");
        flag = TRUE;
        while((c = getchar()) != '\n' && flag) {
            userName[i++] = c;
            if (i == NAME_SIZE) {
                flag = FALSE;
            }
        }
        if(!flag) {
            while(getchar() != '\n');
        }
        userName[NAME_SIZE] = '\0';
    } while (!isValidUserName(userName));
}

boolean
isValidUserName(char *userName) {
    if (strlen(userName) == 0) {
        printf("ERROR: The user name must contain at least one character\n");
        return FALSE;
    }
	return TRUE; //HARDCODEADO!!!
}

void
receiveMessage(char *msg, char *userName) {
	checkRowPosition();
	int i, j;

	i = strlen(userName) + strlen(msg) + 2;

	char totalMessage[NAME_SIZE + MESSAGE_SIZE + 3] = {'\0'};
	strcpy(totalMessage, userName);
	strcat(totalMessage, ": ");
	strcat(totalMessage, msg);

	for(j = 0; j < (MESSAGE_SIZE + NAME_SIZE) && totalMessage[j] != \
	'\0'; j++) {
		if(((i+1) % (CHAT_COLS)) == 0) {
			rowPointer++;
			i = 0;
            checkRowPosition();
		}
		chatMatrix[rowPointer][i++] = totalMessage[j];
	}

	rowPointer++;
}

void
checkRowPosition(void) {
    if(rowPointer == CHAT_ROWS) {
		scrollDown();
		rowPointer=CHAT_ROWS-1;
	}
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
	system("clear");
    printf("Welcome to chat room nbr. %d - PID: %d\n", opt, roomPid);
	printDivision();
	int i, j;
	char msg[MESSAGE_SIZE+1] = {'\0'};
	for(i = 0; i < CHAT_ROWS ; i++) {
		for(j = 0; j < CHAT_COLS ; j++) {
			printf("%c", chatMatrix[i][j]);
		}
		printf("\n");
	}
	i = 0;
	printDivision();
	printf("%s: ", userName);
	i = 0;
	char c;
	boolean flag = TRUE;
	while((c = getchar()) != '\n' && flag) {
		msg[i++] = c;
		if (i == MESSAGE_SIZE) {
			flag = FALSE;
		}
	}
	if(!flag) {
		while(getchar() != '\n');
	}
	msg[MESSAGE_SIZE] = '\0';
	sendMessage(msg, userName);
	welcome(opt, roomPid, userName, pid);
}

void
printDivision(void) {
	int i = 0;
	while(i++ < CHAT_COLS) {
		printf("-");
	}
	printf("\n");
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
	rowPointer = CHAT_ROWS - 1;
}
