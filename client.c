#include "server.h"
#include "client.h"

char userName[NAME_SIZE+1] = {'\0'};
char chatMatrix[CHAT_ROWS][CHAT_COLS] = {'\0'};
int rowPointer = 0;
char roomNumber[MAX_ROOM_DIGITS+1] = {'\0'};
char roomPid[MAX_PID_DIGITS+1] = {'\0'};
pid_t clientPid;

int
main(void) {
	system("clear");
	int fd;
	int amount;
	pid_t* pids;
    clientPid = getpid();
	if((fd = open("server.cfg", O_RDONLY)) < 0) {
		printf("Server is offline. Try connecting again later...\n");
		exit(1);
	} else {
		int rd;
		read(fd, &amount, sizeof(int));
		pids = (pid_t *)malloc(amount*sizeof(pid_t));
		if((rd = read(fd, pids, amount*sizeof(pid_t))) < 0) {
			perror("Failed to read pids on server.cfg");
			close(fd);
			exit(3);
		} else {
			printf("Server online...\nSelect a chat room:\n\n");
			int i;
			for(i = 0; i < amount; i++) {
				printf("Room %d - PID: %d\n", i+1, pids[i]);
			}
            askRoomNumber(amount, pids);
		}
	}
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
chooseUserName(int room) {
    int i;
	char c;
    boolean flag;
    do {
        i = 0;
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
        userName[i] = '\0';
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
askRoomNumber(int rooms, pid_t* pids) {
	int room, i = 0;
    boolean flag;
    char roomAux[MAX_ROOM_DIGITS+1] = {'\0'};
    char c;
	do {
        i = 0;
        printf("\nRoom number: ");
        flag = TRUE;
        while((c = getchar()) != '\n' && flag) {
            roomNumber[i++] = c;
            if (i == MAX_ROOM_DIGITS+1) {
                flag = FALSE;
            }
        }
        if(!flag) {
            while(getchar() != '\n');
        }
        roomNumber[MAX_ROOM_DIGITS] = '\0';
        printf("roomNumber = %s\n", roomNumber);
		room = atoi(roomNumber);
		strcpy(roomPid, itoa(pids[room-1], roomAux));
	}while (!isValidRoomNumber(roomNumber, rooms));
    free(pids);
    chooseUserName(room);
	welcome(room);
}

boolean
checkUserInServer(char *userName, int room, pid_t pid) {
    identifier_t idWrite, idRead;
	char roomNumber[MAX_ROOM_DIGITS+1] = {'\0'};
	boolean hasRead = FALSE;
	boolean userTaken;
	
	char SchatRoom[NAME_SIZE+1] = "SchatRoom";
	char RchatRoom[NAME_SIZE+1] = "RchatRoom";
    
	strcat(SchatRoom, itoa(room, roomNumber));
	strcat(RchatRoom, itoa(room, roomNumber));
	
	char *sIPCname = SchatRoom;
	char *rIPCname = RchatRoom;
	char protocol = USER_CONNECTS;
	info_t protocolInfo;
	info_t userInfo;
	info_t confirmationInfo;
	memcpy(protocolInfo.mtext, &protocol, sizeof(char));
	protocolInfo.mtext[sizeof(char)] = '\0'; 
	protocolInfo.mtype = atoi(roomPid);
	
	int aux, id;
	char result[2];
    char address[NAME_SIZE+1] = "ipc";
    char itoaPid[NAME_SIZE+1] = {'\0'};
    strcat(address, itoa(clientPid, itoaPid));
    id = createIPC(address);
	idWrite = getIdentifier(sIPCname, O_WRONLY, id);
	if(idWrite.fd == -1){
		perror("write IPC open failed");
	}
    if(putInfo(idWrite, &protocolInfo, sizeof(info_t)) < 0){
		perror("Write protocol error");
	}
    usrData_t usrData;
    strcpy(usrData.userName, userName);
    usrData.userPid = pid;
    usrData.connectionTime = time(0);
    memcpy(userInfo.mtext, &usrData, sizeof(usrData_t));
	userInfo.mtype = atoi(roomPid);
	
	if(putInfo(idWrite, &userInfo, sizeof(info_t)) < 0){
		perror("Write user name error");
	}
	while(!hasRead){
        idRead = getIdentifier(rIPCname, O_RDWR, id);
		if(idRead.fd == -1 ){
			perror("read IPC open failed");
		}
		if((aux = getInfo(idRead, &confirmationInfo, sizeof(info_t), atoi(roomPid)*3)) < 0){
			perror("Read failed");
		}
		memcpy(result, confirmationInfo.mtext, 2);
		if(aux > 0){
			if((strcmp(result, "y")==0)){
				userTaken = FALSE;
			}
			else{
                system("clear");
                printf("That user name is in use. Please select another one.\n");
				userTaken = TRUE;
			}
			hasRead = TRUE;
		}
	}
    strcpy(itoaPid, "rm -rf ");
    strcat(itoaPid, address);
    system(itoaPid);
	return userTaken;
}

boolean
isValidUserName(char *userName, int room) {
    if (strlen(userName) == 0) {
        printf("ERROR: The user name must contain at least one character\n");
        return FALSE;
    }
	return TRUE;
}

void
welcome(int opt) {
    system("clear");
    printf("Welcome to chat room nbr. %d\n", opt);
    printDivision();
    pid_t aux;
    switch(aux = fork()){
        case -1: {
            perror("Failed to fork on client.");
            exit(1);
        }
        case 0:
            prompt();
            break;
        default:
            waitForMessages();
            break;
    }
}

void
waitForMessages(void) {
    char rmsg[NAME_SIZE+1] = "r_msg";
    int aux;
    identifier_t id;
    char roomAux[MAX_PID_DIGITS+1] = {'\0'};
    info_t messageInfo;
    
    strcat(rmsg, itoa(getpid(), roomAux));
    if((id.fd = createIPC(rmsg)) == -1){
		perror("Creating rmsg IPC error");
        exit(0);
	}
    id = getIdentifier(rmsg, O_RDWR, id.fd);
    if(id.fd == -1) {
		perror("rmsg IPC open failed");
        exit(0);
	}
    while (TRUE) {
        message_t message;
        if((aux = getInfo(id, &messageInfo, sizeof(info_t), clientPid*2)) < 0){
            perror("Failed to read user name");
            exit(0);
        } else {
			memcpy(&message, messageInfo.mtext, sizeof(message_t));
            printf("%s says: %s\n", message.userName, message.msg);
            if (strcmp(message.msg, "You have left the chat room...") == 0) {
                exit(0);
            }
        }
    }
}

void
prompt(void) {
    char par[MAX_PID_DIGITS+1] = {'\0'};
    execl("prompt", userName, itoa(clientPid, par), roomNumber, roomPid, NULL);
}

void
printDivision(void) {
	int i = 0;
	while(i++ < CHAT_COLS) {
		printf("-");
	}
	printf("\n");
}
