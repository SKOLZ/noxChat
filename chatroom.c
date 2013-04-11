#include "server.h"

int roomNumber;
int roomPid;
int clientNumber;
int *clientPids;
usrData_t *users;
int SchatRoomFD;

int
main(int argc, char **argv) {
    static struct sigaction act;
    void catchint(int);
    
    act.sa_handler = catchint;
    sigfillset(&(act.sa_mask));
    
    sigaction(SIGINT, &act, NULL);
    
	if(argc != 2) {
		perror("Invalid amount of arguments");
		exit(1);
	}
	roomNumber = atoi(argv[0]);
	roomPid = atoi(argv[1]);

    printf("\nChat room nbr. %d has been created with PID %d\n", roomNumber + 1, roomPid);
    
	int aux;

    char SchatRoom[NAME_SIZE+1] = "SchatRoom";
    char RchatRoom[NAME_SIZE+1] = "RchatRoom";
    createFifo(SchatRoom);
    createFifo(RchatRoom);
	welcomeUsers(SchatRoom, RchatRoom);
}

void
catchint(int signo) {
    message_t *serverMessage = (message_t *)malloc(sizeof(message_t));
    strcpy(serverMessage->userName, "DEDICATED SERVER");
    char msg[MESSAGE_SIZE+1] = "Server execution has been terminated suddenly. Please try connecting again later...\nERROR CODE: ";
    char aux[6] = {'\0'};
    strcat(msg, itoa(signo, aux));
    strcpy(serverMessage->msg, msg);
    broadcast(serverMessage);
}

void
createFifo(char *fifoName) {
    char roomAux[MAX_ROOM_DIGITS+1] = {'\0'};
    
	strcat(fifoName, itoa(roomNumber+1, roomAux));
    int fd;
	/*--creating fifos--*/
	if((mkfifo(fifoName, 0666)) == -1){ 
		perror("creating fifo read error");
        exit(0);
	}
}

void
welcomeUsers(char *fifoRead, char *fifoWrite){
	int fdRead, fdWrite, aux1, aux2;
    char protocol;
    
	/*--begining reading user name--*/
	if((fdRead = open(fifoRead, O_RDWR)) < 0){
		perror("Fifo open failed");
        exit(0);
	}
    SchatRoomFD = fdRead;
	while(TRUE){
        if((aux1 = read(fdRead, &protocol, sizeof(char))) < 0){
			perror("Failed to read protocol.");
            exit(0);
		}
        if(protocol == USER_MESSAGE) {
            message_t *message = (message_t *)malloc(sizeof(message_t));
            if((aux1 = read(fdRead, message, sizeof(message_t))) < 0){
                perror("Failed to read messagef structure.");
                exit(0);
            }
            if (!isCommand(message)) {
                broadcast(message);
            }
        } else if(protocol == USER_CONNECTS) {
            usrData_t *usrData = (usrData_t *)malloc(sizeof(usrData_t));
            if((aux2 = read(fdRead, usrData, sizeof(usrData_t))) < 0){
                perror("Failed to read user structure.");
                exit(0);
            }
            if(aux1 > 0 && aux2 > 0) {
                printf("\nSERVER MESSAGE: User \"%s\" has joined room number %d - User PID = %d\n", usrData->userName, roomNumber+1, usrData->userPid);
                
                /*--ending reading user name--*/
                /*--begining writing user Available--*/
                if((fdWrite = open(fifoWrite, O_WRONLY)) < 0){
                    perror("Oppening name Available in fifo failed");
                }
                if(uniqueUser(usrData->userName)) {
                    addToUserList(usrData);
                    if(write(fdWrite, "y", 2) == -1){
                        perror("Writing name Available failed");
                    }
                    /*-- creatin dedicated server for user--*/
                    switch(fork()){
                        case -1: {
                            perror("Failed to fork");
                            exit(2);
                            break;
                        }
                        case 0: {
                            listenToUser(usrData->userName, usrData->userPid, getpid());
                            break;
                        }
                        default: {
                            break;
                        }
                    }
                }
                else{
                    if(write(fdWrite, "n", 2) == -1){
                        perror("Writing name Availabl failed");
                    }
                }
                close(fdWrite);
                /*ending writing user available--*/
                
            }
        }
    }
}

boolean
isCommand(message_t *message) {
    if (!strcmp(message->msg, COMMAND_COST)) {
        message_t *serverMessage = (message_t *)malloc(sizeof(message_t));
        strcpy(serverMessage->userName, "DEDICATED SERVER");
        char msg[MESSAGE_SIZE+1] = "Current cost is ";
        char aux[10] = {'\0'};
        strcat(msg, itoa(((time(0)-getConnectionTime(message->userName))/60)*PRICE, aux));
        strcpy(serverMessage->msg, msg);
        sendMessageToUser(getUserPid(message->userName), serverMessage);
        free(serverMessage);
        return TRUE;
    } else if(!strcmp(message->msg, COMMAND_QUIT)) {
        message_t *serverMessage = (message_t *)malloc(sizeof(message_t));
        strcpy(serverMessage->userName, "DEDICATED SERVER");
        strcpy(serverMessage->msg, "You have left the chat room. Press ctrl. + C to terminate program...");
        sendMessageToUser(getUserPid(message->userName), serverMessage);
        removeFromUserList(message->userName);
        printf("SERVER MESSAGE: User %s has left the chat room number %d\n", message->userName, roomNumber+1);
        free(serverMessage);
        return TRUE;
    }
    return FALSE;
}

pid_t
getUserPid(char *userName) {
    usrData_t *curr = users;
    while (curr != NULL) {
        if (!strcmp(userName, curr->userName)) {
            return curr->userPid;
        }
        curr = curr->next;
    }
    return -1;
}

time_t
getConnectionTime(char *userName) {
    usrData_t *curr = users;
    while (curr != NULL) {
        if (!strcmp(userName, curr->userName)) {
            return curr->connectionTime;
        }
        curr = curr->next;
    }
    return -1;
}

void
addToUserList(usrData_t *usrData) {
    if (users == NULL) {
        users = usrData;
    } else {
        usrData_t *curr = users;
        while (curr->next != NULL) {
            curr = curr->next;
        }
        curr->next = usrData;
    }
    //showUsers();
}

void
removeFromUserList(char *userName) {
    usrData_t *curr = users;
    usrData_t *prev = NULL;
    while (curr != NULL) {
        if (!strcmp(curr->userName, userName)) {
            if (prev != NULL) {
                prev->next = curr->next;
            } else {
                users = curr->next;
            }
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void
showUsers(void) {
    printf("Users in room %d:\n", roomNumber);
    usrData_t *curr = users;
    while (curr != NULL) {
        printf("\nUsuario: %s\n",curr->userName);
        printf("PID: %d\n", curr->userPid);
        curr = curr->next;
    }
}

void
listenToUser(char *userName, pid_t userPid, pid_t dsPid) {
    char ds[NAME_SIZE+1] = {'\0'};
    char roomAux[MAX_PID_DIGITS+1] = {'\0'};
    char protocol = USER_MESSAGE;
    boolean hasRead = FALSE;
	strcpy(ds, "dsr");
    strcat(ds, itoa(userPid, roomAux));

	printf("\nDedicated server with pid %d has been created for user %s (%d)\n",\
           dsPid, userName, userPid);
	/*--creating fifos--*/
	if(mkfifo(ds, 0666) == -1){
		perror("creating fifo read error");
        exit(0);
	}
    int fd, aux;
    if((fd = open(ds, O_RDWR)) < 0){
		perror("fifo open failed");
        exit(0);
	}
    message_t *message = (message_t *)malloc(sizeof(message_t));
    while (TRUE) {
        if((aux = read(fd, message, sizeof(message_t))) < 0){
            perror("Failed to read user name.");
            exit(0);
        } else if (aux > 0) {
            if((aux = write(SchatRoomFD, &protocol, sizeof(char))) < 0){
                perror("Failed to write protocol.");
                exit(0);
            }
            if((aux = write(SchatRoomFD, message, sizeof(message_t))) < 0){
                perror("Failed to write user message.");
                exit(0);
            }
        }
    }
}

void
broadcast(message_t *message) {
    usrData_t *curr = users;
    int fd, aux;
    while (curr != NULL) {
        if (strcmp(curr->userName, message->userName) != 0) {
            sendMessageToUser(getUserPid(curr->userName), message);
        }
        curr = curr->next;
    }
}

void
sendMessageToUser(pid_t pid, message_t *message) {
    int fd, aux;
    char fifo[NAME_SIZE+1] = {'\0'};
    char userPid[MAX_PID_DIGITS+1] = {'\0'};
    strcpy(fifo, "r_msg");
    strcat(fifo, itoa(pid, userPid));
    if((fd = open(fifo, O_WRONLY)) < 0){
        perror("fifo open failed");
        exit(0);
    }
    if((aux = write(fd, message, sizeof(message_t))) < 0){
        perror("Failed to write user message.");
        exit(0);
    }
    close(fd);
}

boolean
uniqueUser(char* userName){
	return TRUE; //HARCODEADOOOOO!!!!!!!!!!
}
