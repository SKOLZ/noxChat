#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include "server.h"
#include "client.h"

char userName[NAME_SIZE+1];
char chatMatrix[CHAT_ROWS][CHAT_COLS] = {'\0'};
int rowPointer = 0;
char roomNumber[MAX_ROOM_DIGITS+1] = {'\0'};
char roomPid[MAX_PID_DIGITS+1] = {'\0'};

int
main(void) {
	system("clear");
	int fd;
	int amount[1];
	pid_t* pids;
	if((fd = open("server.cfg", O_RDONLY)) < 0) {
		printf("Server is offline. Try connecting again later...\n");
		exit(1);
	} else {
		int rd;
		read(fd, amount, (size_t)sizeof(int));
		pids = malloc(amount[0]*sizeof(pid_t));
		if((rd = read(fd, pids, (size_t)amount[0]*sizeof(pid_t))) < \
            (size_t)amount[0]*sizeof(pid_t)) {
			perror("Failed to read pids on server.cfg");
			close(fd);
			exit(3);
		} else {
			printf("Server online...\nSelect a chat room:\n\n");
			int i;
			for(i = 0; i < amount[0]; i++) {
                itoa(pids[i], roomPid);
				printf("Room %d - PID: %d\n", i+1, pids[i]);
			}
		}
	}
	
	askRoomNumber(amount[0], pids);
	exit(0);
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
connect(int room) {
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
    } while (!isValidUserName(userName, room) || checkUserInServer(userName, room, getpid()));
}

void
resetOption(char *opt) {
    int i;
    for (i = 0; i < MAX_ROOM_DIGITS; i++) {
        opt[i] = '\0';
    }
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
askRoomNumber(int rooms, int *pids) {
	int room, i = 0;
    boolean flag;
    char c;
	do {
        i = 0;
        printf("\nRoom number: ");
        flag = TRUE;
        while((c = getchar()) != '\n' && flag) {
            roomNumber[i++] = c;
            if (i == MAX_ROOM_DIGITS) {
                flag = FALSE;
            }
        }
        if(!flag) {
            while(getchar() != '\n');
        }
        roomNumber[MAX_ROOM_DIGITS] = '\0';
		room = atoi(roomNumber);
	}while (!isValidRoomNumber(roomNumber, rooms));
    connect(room);
	welcome(room, pids[room], userName, getpid());
}

boolean
checkUserInServer(char *userName, int room, pid_t pid) {
	int fdWrite;
	int fdRead;
	char roomNumber[MAX_ROOM_DIGITS];
	boolean hasRead = FALSE;
	boolean userAvailable;
	
	char SchatRoom[NAME_SIZE];
	char RchatRoom[NAME_SIZE];
	char aPid[MAX_PID_DIGITS];
    
	strcpy(SchatRoom, "SchatRoom"); 
	strcpy(RchatRoom, "RchatRoom");
	strcat(SchatRoom, itoa(room, roomNumber));
	strcat(RchatRoom, itoa(room, roomNumber));
	
	char *sfifo = SchatRoom;
	char *rfifo = RchatRoom;
	
	int aux;
	char result[NAME_SIZE+1];
	/*-- open read before --*/
	if((fdRead = open(rfifo, O_RDWR)) < 0){
		perror("read fifo open failed");
	}
	/*-- begining writing name in fifo --*/
	if((fdWrite = open(sfifo, O_WRONLY)) < 0){
		perror("write fifo open failed");
	}
    if((write(fdWrite, USER_CONNECTS, sizeof(USER_CONNECTS))) < 0){
		perror("write name error");
	}
	if((write(fdWrite, userName, NAME_SIZE+1)) < 0){
		perror("write name error");
	}
    if((write(fdWrite, itoa(pid, aPid), sizeof(pid_t)))== -1){
		perror("write name error");
	}
	close(fdWrite);
	/*ending writing name in fifo--*/
	/*begining reading user Available from fifo--*/
	while(!hasRead){
		if((aux = read(fdRead, result, NAME_SIZE)) < 0){
			printf("%d", fdRead);
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
isValidUserName(char *userName, int room) {
    if (strlen(userName) == 0) {
        printf("ERROR: The user name must contain at least one character\n");
        return FALSE;
    }
	return TRUE; //HARDCODEADO!!!
}

void
clearLastRow(void) {
	int i;
	for(i = 0; i < CHAT_COLS ; i++) {
		chatMatrix[CHAT_ROWS-1][i] = '\0';
	}
}

void
welcome(int opt, pid_t roomPid, char* userName, pid_t pid) {
    system("clear");
    printf("Welcome to chat room nbr. %d - PID: %d\n", opt, roomPid);
    printDivision();
    pid_t aux;
    switch(aux = fork()){
        case -1: {
            perror("Failed to Fork");
            exit(1);
        }
        case 0:
            waitForMessages();
            break;
        default:
            prompt();
            break;
    }
}

void
waitForMessages(void) {
    char rmsg[NAME_SIZE] = {'\0'};
    int fd, aux;
    char roomAux[MAX_PID_DIGITS+1] = {'\0'};
    boolean hasRead = FALSE;
	strcpy(rmsg, "r_msg");
    strcat(rmsg, itoa(getpid(), roomAux));

    if(mkfifo(rmsg, 0666) == -1){
		perror("creating fifo read error");
        exit(0);
	}
    
	/*--creating fifos--*/
    if((fd = open(rmsg, O_RDWR)) < 0) {
		perror("Fifo open failed");
        exit(0);
	}
    while (TRUE) {
        message_t *message;
        if((aux = read(fd, message, sizeof(message_t))) < 0){
            perror("Failed to read user name.");
            exit(0);
        } else if (aux > 0) {
            printf("%s: %s", message->userName, message->msg);
        }
    }
}

void
prompt(void) {
    char par[MAX_PID_DIGITS+1] = {'\0'};
    execl("prompt", userName, itoa(getpid(), par), roomNumber, roomPid, NULL);
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
scrollDown(void) {
	int i, j;
	for(i = 0; i < CHAT_ROWS - 1; i++) {
		for(j = 0; j < CHAT_COLS; j++) {
			chatMatrix[i][j] = chatMatrix[i+1][j];
		}
	}
	clearLastRow();
}

int
tabulateChat(void) {
    int i;
    int lenght = strlen(userName) + 2;
    for (i = 0; i < lenght; i++) {
        chatMatrix[rowPointer][i] = ' ';
    }
    return lenght;
}

int
checkRowPosition(void) {
    if(rowPointer == CHAT_ROWS-1) {
		scrollDown();
	} else {
        rowPointer++;
    }
}
