#include "server.h"

int roomNumber;
int roomPid;
int clientNumber;
usrData_t *users;
int SchatRoomID;
int clientsInRoom;

int
main(int argc, char **argv) {
    static struct sigaction act;
    void catchint(int);
	char roomAux[MAX_ROOM_DIGITS+1] = {'\0'};
    
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

	char SchatRoom[NAME_SIZE+1] = "SchatRoom";
    char RchatRoom[NAME_SIZE+1] = "RchatRoom";
	strcat(SchatRoom, itoa(roomNumber+1, roomAux));
	strcat(RchatRoom, itoa(roomNumber+1, roomAux));
	
    if(createIPC(SchatRoom) == -1){
		perror("creating fifo write error");
        exit(0);
    }
    if(createIPC(RchatRoom) == -1){
		perror("creating fifo read error");
        exit(0);
    }
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
    freeUserList();
    free(serverMessage);
}

void
freeUserList(void) {
    usrData_t *tmp;

    while (users != NULL)
    {
        tmp = users;
        users = users->next;
        free(tmp);
    }
}

void
welcomeUsers(char *reader, char *writer){
	int readerID, writerID, aux1, aux2;
    char protocol;
	/*--begining reading user name--*/
	if((readerID = getIdentifier(reader, O_RDWR)) < 0){
		perror("Fifo open failed");
        exit(0);
    }
    SchatRoomID = readerID;
	while(TRUE){
		if((aux1 = getInfo(readerID, &protocol, sizeof(char), roomPid)) < 0){
			perror("Failed to read protocol.");
			exit(0);
		}
        if(protocol == USER_MESSAGE) {
            message_t *message = (message_t *)malloc(sizeof(message_t));
            if((aux1 = getInfo(readerID, message, sizeof(message_t), roomPid)) < 0){
				perror("Failed to read protocol.");
				exit(0);
			}
            if (!isCommand(message)) {
                broadcast(message);
            }
            free(message);
        } else if(protocol == USER_CONNECTS) {
            usrData_t *usrData = (usrData_t *)malloc(sizeof(usrData_t));
            if((aux2 = getInfo(readerID, usrData, sizeof(usrData_t), roomPid)) < 0){
				perror("Failed to read protocol.");
				exit(0);
			}
            if(aux1 > 0 && aux2 > 0) {
                if((writerID = getIdentifier(writer, O_WRONLY)) < 0){
					perror("Fifo open failed");
					exit(0);
				}
                if(uniqueUser(usrData->userName)) {
                    addToUserList(usrData);
                    printf("\nSERVER MESSAGE: User \"%s\" has joined room number %d - User PID = %d\n", usrData->userName, roomNumber+1, usrData->userPid);
					if(putInfo(writerID, "y", 2, roomPid) == -1){
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
                } else {
					if(putInfo(writerID, "n", 2, roomPid) == -1){
						perror("Writing name Available failed");
                    }
                }
                endIPC(writerID);
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
    } else if(!strcmp(message->msg, COMMAND_USERS)) {
        message_t *serverMessage = (message_t *)malloc(sizeof(message_t));
        strcpy(serverMessage->userName, "DEDICATED SERVER");
        int msgLenth = 0;
        usrData_t *curr = users;
        strcpy(serverMessage->msg, "\n\nUsers in current chat room are:\n-------------------------------\n");
        while (curr != NULL) {
            if (msgLenth > MESSAGE_SIZE-3 || msgLenth + strlen(curr->userName) > MESSAGE_SIZE ) {
                strcat(serverMessage->msg, "...");
                sendMessageToUser(getUserPid(message->userName), serverMessage);
                return;
            } else {
                strcat(serverMessage->msg, curr->userName);
                strcat(serverMessage->msg, "\n");
            }
            msgLenth += strlen(curr->userName)+1;
            curr = curr->next;
        }
        serverMessage->msg[strlen(serverMessage->msg)] = '\0';
        sendMessageToUser(getUserPid(message->userName), serverMessage);
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
    message_t *serverMessage = (message_t *)malloc(sizeof(message_t));
    
    if (users == NULL) {
        users = usrData;
    } else {
        usrData_t *curr = users;
        while (curr->next != NULL) {
            curr = curr->next;
        }
        curr->next = usrData;
    }
    strcpy(serverMessage->msg, "Hello! I've joined your chat room!");
    strcpy(serverMessage->userName, usrData->userName);
    broadcast(serverMessage);
    free(serverMessage);
    clientsInRoom++;
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
            clientsInRoom--;
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void
getUsers(void) {
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
	if(createIPC(ds) == -1){
		perror("creating fifo read error");
        exit(0);
    }
    int ID, aux;
    if((ID = getIdentifier(ds, O_RDWR)) < 0){
		perror("Fifo open failed");
        exit(0);
    }
    message_t *message = (message_t *)malloc(sizeof(message_t));
    while (TRUE) {
        if((aux = getInfo(ID, message, sizeof(message_t), userPid)) < 0){
            perror("Failed to read user name.");
            free(message);
            exit(0);
        } else if (aux > 0) {
            if((aux = putInfo(SchatRoomID, &protocol, sizeof(char), roomPid)) < 0){
                perror("Failed to write protocol.");
                free(message);
                exit(0);
            }
            if((aux = putInfo(SchatRoomID, message, sizeof(message_t), roomPid)) < 0){
                perror("Failed to write user message.");
                free(message);
                exit(0);
            }
        }
    }
}

void
broadcast(message_t *message) {
    usrData_t *curr = users;
    int id, aux;
    while (curr != NULL) {
        if (strcmp(curr->userName, message->userName) != 0) {
            sendMessageToUser(getUserPid(curr->userName), message);
        }
        curr = curr->next;
    }
}

void
sendMessageToUser(pid_t pid, message_t *message) {
    int id, aux;
    char IPCname[NAME_SIZE+1] = {'\0'};
    char userPid[MAX_PID_DIGITS+1] = {'\0'};
    strcpy(IPCname, "r_msg");
    strcat(IPCname, itoa(pid, userPid));
    if((id = getIdentifier(IPCname, O_WRONLY)) < 0){
        perror("fifo open failed");
        exit(0);
    }
    if((aux = putInfo(id, message, sizeof(message_t), pid*2)) < 0){
        perror("Failed to write user message.");
        exit(0);
    }
    endIPC(id);
}

boolean
uniqueUser(char* userName){
    usrData_t *curr = users;
    while (curr != NULL) {
        if (!strcmp(curr->userName, userName)) {
            return FALSE;
        }
        curr = curr->next;
    }
    return TRUE;
}
