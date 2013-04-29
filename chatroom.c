#include "server.h"

int roomNumber;
int roomPid;
int clientNumber;
usrData_t *users;
identifier_t SchatRoomID;
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
	
    int scr, rcr;
    
    if((scr = createIPC(SchatRoom)) == -1){
		perror("creating IPC write error");
        exit(0);
    }
    if((rcr = createIPC(RchatRoom)) == -1){
		perror("creating IPC read error");
        exit(0);
    }
	welcomeUsers(SchatRoom, RchatRoom, scr, rcr);
}

void
catchint(int signo) {
    message_t serverMessage;
    strcpy(serverMessage.userName, "DEDICATED SERVER");
    char msg[MESSAGE_SIZE+1] = "Server execution has been terminated suddenly. Please try connecting again later...\nERROR CODE: ";
    char aux[6] = {'\0'};
    strcat(msg, itoa(signo, aux));
    strcpy(serverMessage.msg, msg);
    broadcast(serverMessage);
    freeUserList();
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
welcomeUsers(char *reader, char *writer, int idr, int idw){
	int aux1, aux2;
    identifier_t readerID, writerID;
    char protocol;
    info_t confirmationInfo;
    readerID = getIdentifier(reader, O_RDWR, idr);
	if(readerID.fd == -1) {
		perror("IPC open failed");
        exit(0);
    }
    SchatRoomID = readerID;
	while(TRUE){
		info_t protocolInfo;
		if((aux1 = getInfo(readerID, &protocolInfo, sizeof(info_t), roomPid)) < 0){
			perror("Failed to read protocol.");
			exit(0);
		}
		memcpy(&protocol, protocolInfo.mtext, sizeof(char));
        if(protocol == USER_MESSAGE) {
			info_t messageInfo;
            message_t message;
            if((aux1 = getInfo(readerID, &messageInfo, sizeof(info_t), roomPid)) < 0){
				perror("Failed to read protocol.");
				exit(0);
			}
			memcpy(&message, &messageInfo.mtext, sizeof(message_t));
            if (!isCommand(message)) {
                broadcast(message);
            }
        } else if(protocol == USER_CONNECTS) {
			info_t userInfo;
            usrData_t *usrData = (usrData_t *)malloc(sizeof(usrData_t));
            if((aux2 = getInfo(readerID, &userInfo, sizeof(info_t), roomPid)) < 0){
				perror("Failed to read protocol.");
				exit(0);
			}
			memcpy(usrData, userInfo.mtext, sizeof(usrData_t));
            usrData->next = NULL;
            if(aux1 > 0 && aux2 > 0) {
                /*int ids = createIPC("server");
                identifier_t idsi = getIdentifier("ipc", O_WRONLY, ids);*/
                writerID = getIdentifier(writer, O_WRONLY, idw);
                if(writerID.fd == -1){
					perror("IPC open failed");
					exit(0);
				}
                if(uniqueUser(usrData->userName)) {
                    addToUserList(usrData);
                    printf("\nSERVER MESSAGE: User \"%s\" has joined room number %d - User PID = %d\n", usrData->userName, roomNumber+1, usrData->userPid);
                    memcpy(confirmationInfo.mtext, "y", 2);
                    confirmationInfo.mtype = roomPid*3;
                    if(putInfo(writerID, &confirmationInfo, sizeof(info_t)) == -1){
						perror("Writing name Available failed");
                    }
                    switch(fork()) {
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
					memcpy(confirmationInfo.mtext, "n", 2);
                    confirmationInfo.mtype = roomPid*3;
					if(putInfo(writerID, &confirmationInfo, sizeof(info_t)) == -1){
						perror("Writing name Available failed");
                    }
                }
                endIPC(writerID);
            }
        } else {
            perror("Protocol error\n");
            exit(1);
        }
    }
}

boolean
isCommand(message_t message) {
    if (!strcmp(message.msg, COMMAND_COST)) {
        message_t serverMessage;
        strcpy(serverMessage.userName, "DEDICATED SERVER");
        char msg[MESSAGE_SIZE+1] = "Current cost is ";
        char aux[10] = {'\0'};
        strcat(msg, itoa(((time(0)-getConnectionTime(message.userName))/60)*PRICE, aux));
        strcpy(serverMessage.msg, msg);
        sendMessageToUser(getUserPid(message.userName), serverMessage);
        return TRUE;
    } else if(!strcmp(message.msg, COMMAND_QUIT)) {
        removeFromUserList(message.userName);
        message_t serverMessage;
        strcpy(serverMessage.userName, "DEDICATED SERVER");
        strcpy(serverMessage.msg, "You have left the chat room...");
        sendMessageToUser(getUserPid(message.userName), serverMessage);
        strcpy(serverMessage.msg, message.userName);
        strcat(serverMessage.msg, " has left the chat room.");
        broadcast(serverMessage);
        return TRUE;
    } else if(!strcmp(message.msg, COMMAND_USERS)) {
        message_t serverMessage;
        strcpy(serverMessage.userName, "DEDICATED SERVER");
        int msgLenth = 0;
        usrData_t *curr = users;
        strcpy(serverMessage.msg, "\n\nUsers in current chat room are:\n-------------------------------\n");
        while (curr != NULL) {
            if (msgLenth > MESSAGE_SIZE-3 || msgLenth + strlen(curr->userName) > MESSAGE_SIZE ) {
                strcat(serverMessage.msg, "...");
                sendMessageToUser(getUserPid(message.userName), serverMessage);
                return;
            } else {
                strcat(serverMessage.msg, curr->userName);
                strcat(serverMessage.msg, "\n");
            }
            msgLenth += strlen(curr->userName)+1;
            curr = curr->next;
        }
        serverMessage.msg[strlen(serverMessage.msg)] = '\0';
        sendMessageToUser(getUserPid(message.userName), serverMessage);
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
    message_t serverMessage;
    
    if (users == NULL) {
        users = usrData;
    } else {
        usrData_t *curr = users;
        while (curr->next != NULL) {
            curr = curr->next;
        }
        curr->next = usrData;
    }
    strcpy(serverMessage.msg, usrData->userName);
    strcat(serverMessage.msg, " has joined the chat room.");
    strcpy(serverMessage.userName, "SERVER MESSAGE");
    broadcast(serverMessage);
    clientsInRoom++;
}

void
removeFromUserList(char *userName) {
    usrData_t *curr = users;
    usrData_t *prev = NULL;
    message_t serverMessage;
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
    strcpy(serverMessage.msg, userName);
    strcat(serverMessage.msg, " has left the chat room.");
    strcpy(serverMessage.userName, "SERVER MESSAGE");
    broadcast(serverMessage);
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
    int aux, dsrid;
    identifier_t ID;
    boolean hasRead = FALSE;
    info_t protocolInfo;
    info_t messageInfo;
	strcpy(ds, "dsr");
    strcat(ds, itoa(userPid, roomAux));

	printf("\nDedicated server with pid %d has been created for user %s (%d)\n",\
           dsPid, userName, userPid);
	if((dsrid = createIPC(ds)) == -1){
		perror("creating IPC read error");
        exit(0);
    }
    ID = getIdentifier(ds, O_RDWR, dsrid);
    if(ID.fd == -1){
		perror("IPC fasfasfsdfasfopen failed");
        exit(0);
    }
    while (TRUE) {
        if((aux = getInfo(ID, &messageInfo, sizeof(info_t), userPid)) < 0){
            perror("Failed to read user name.");
            exit(0);
        } else if (aux > 0) {
			memcpy(protocolInfo.mtext, &protocol, 1);
            protocolInfo.mtype = roomPid;
            if((aux = putInfo(SchatRoomID, &protocolInfo, sizeof(info_t))) < 0){
                perror("Failed to write protocol.");
                exit(0);
            }
            messageInfo.mtype = roomPid;
            if((aux = putInfo(SchatRoomID, &messageInfo, sizeof(info_t))) < 0){
                perror("Failed to write user message.");
                exit(0);
            }
        }
    }
}

void
broadcast(message_t message) {
    usrData_t *curr = users;
    int id, aux;
    while (curr != NULL) {
        if (strcmp(curr->userName, message.userName) != 0) {
            sendMessageToUser(getUserPid(curr->userName), message);
        }
        curr = curr->next;
    }
}

void
sendMessageToUser(pid_t pid, message_t message) {
    int aux;
    identifier_t id;
    char IPCname[NAME_SIZE+1] = {'\0'};
    char userPid[MAX_PID_DIGITS+1] = {'\0'};
    info_t messageInfo;
    strcpy(IPCname, "r_msg");
    strcat(IPCname, itoa(pid, userPid));
    id = getIdentifier(IPCname, O_WRONLY, SchatRoomID.fd);
    if(id.fd == -1){
        return;
    }
    memcpy(messageInfo.mtext, &message, sizeof(message_t));
    messageInfo.mtype = pid*2;
    if((aux = putInfo(id, &messageInfo, sizeof(info_t))) < 0){
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
