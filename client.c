#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include "server.h"

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
	char opt[MAX_ROOM_DIGITS+1] = {'\0'};
	int nOpt;
	do {
		printf("\nRoom number: ");
		scanf("%s", opt);
		opt[MAX_ROOM_DIGITS] = '\0';
		nOpt = atoi(opt);
	}while (!isNumber(opt) || nOpt < 1 || nOpt > amount[0]);
	connect(nOpt);
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
connect(int room) {
	do {
		printf("User name: ");
		scanf("%s", userName);
		while(getchar()!='\n');
		userName[NAME_SIZE] = '\0';
	} while(!validateUserName(userName) || checkUserInServer("pepe"/*userName*/, room));
	
}

boolean
checkUserInServer(char *userName, int room){
	int fdWrite;
	int fdRead;
	char roomNumber[2];
	boolean hasRead = FALSE;
	boolean userAvailable;
	
	char SchatRoom[32];
	char RchatRoom[32];
	
	strcpy(SchatRoom, "SchatRoom"); 
	strcpy(RchatRoom, "Rchatroom");
	strcat(SchatRoom, itoa(room, roomNumber));
	strcat(RchatRoom, itoa(room, roomNumber));
	
	char *sfifo = SchatRoom;
	char *rfifo = RchatRoom;
	
	int aux;
	char *result;
	
	/*-- begining writing name in fifo --*/
	if((fdWrite = open(sfifo, O_WRONLY)) < 0){
		perror("write fifo open failed");
	}
	if((write(fdWrite, userName, NAME_SIZE+1))== -1){
		perror("write name error");
	}
	close(fdWrite);
	/*ending writing name in fifo--*/
	/*begining reading user Available from fifo--*/
	if((fdRead = open(rfifo, O_RDWR)) < 0){
		perror("read fifo open failed");
	}
	while(!hasRead){
		if((aux = read(fdRead, result, NAME_SIZE)) < 0){
			perror("read failed");
		}
		if(aux > 0){
			if((strcmp(result, "y")==0)){
				userAvailable = FALSE;
			}
			else{
				userAvailable = TRUE;
			}
			hasRead = TRUE;
		}
	}
	close(fdWrite);
	/*--ending reading user Availble from fifo--*/
	return userAvailable;
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
		}
		chatMatrix[rowPointer][i++] = totalMessage[j];
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
	int flag = 1;
	while((c = getchar()) != '\n' && flag) {
		msg[i++] = c;
		if (i == MESSAGE_SIZE) {
			flag = 0;
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




