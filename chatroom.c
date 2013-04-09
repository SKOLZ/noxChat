#include "server.h"

int roomNumber;
int roomPid;
int clientNumber;
int *clientPids;
usrData_t *users;
int SchatRoomFD;

int
main(int argc, char **argv) {
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
	while(TRUE){
		welcomeUsers(SchatRoom, RchatRoom);
	}
}

void
createFifo(char *fifoName) {
    char roomAux[MAX_ROOM_DIGITS+1] = {'\0'};
    
	strcat(fifoName, itoa(roomNumber+1, roomAux));
    int fd;
	/*--creating fifos--*/
	if((SchatRoomFD = mkfifo(fifoName, 0666)) == -1){ 
		perror("creating fifo read error");
        exit(0);
	}
}

void
welcomeUsers(char *fifoRead, char *fifoWrite){
	int fdRead, fdWrite, aux1, aux2, aux3;
	boolean hasRead = FALSE;
    char userName[NAME_SIZE+1] = {'\0'};
    char *pid = malloc(sizeof(pid_t));
    char protocol[2];

	/*--begining reading user name--*/
	if((fdRead = open(fifoRead, O_RDWR)) < 0){
		perror("fifo open failed");
        exit(0);
	}
	while(!hasRead) {
        if((aux1 = read(fdRead, protocol, 2)) < 0){
			perror("Failed to read protocol.");
            exit(0);
		}
           if(!strcmp(protocol, USER_MESSAGE)) {
               message_t *message;
               if((aux1 = read(fdRead, message, sizeof(message_t))) < 0){
                   perror("Failed to read message structure.");
                   exit(0);
               }
               broadcast(message);
           }
		if((aux2 = read(fdRead, userName, NAME_SIZE+1)) < 0){
			perror("Failed to read user name.");
            exit(0);
		}
        if((aux3 = read(fdRead, pid, sizeof(pid_t))) < 0){
			perror("Failed to read pid.");
            exit(0);
		}
		if(aux1 > 0 && aux2 > 0 && aux3 > 0) {
			printf("\nSERVER MESSAGE: User \"%s\" has joined room number %d - User PID = %s\n", userName, roomNumber+1, pid);
            
			hasRead = TRUE;
			/*--ending reading user name--*/
			/*--begining writing user Available--*/
			if((fdWrite = open(fifoWrite, O_WRONLY)) < 0){
				perror("Oppening name Available in fifo failed");
			}
			if(uniqueUser(userName)) {
				close(fdRead);
				addToUserList(userName, atoi(pid));
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
						listenToUser(userName, atoi(pid), getpid());
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

void
addToUserList(char *userName, pid_t pid) {
    if (users == NULL) {
        users = malloc(sizeof(usrData_t));
        strcpy(users->usrName, userName);
        users->pid = pid;
    } else {
        usrData_t *curr = users;
        while (curr->next != NULL) {
            curr = curr->next;
        }
        curr->next = malloc(sizeof(usrData_t));
        strcpy(curr->next->usrName, userName);
        curr->next->pid = pid;
    }
    showUsers();
}

void
showUsers(void) {
    usrData_t *curr = users;
    while (curr->next != NULL) {
        printf("\nUsuario: %s\n",curr->usrName);
        printf("PID: %d\n", curr->pid);
        printf("--------------------------------\n");
        curr = curr->next;
    }
}

void
listenToUser(char *userName, pid_t userPid, pid_t dsPid) {
    char ds[NAME_SIZE+1] = {'\0'};
    
    char msg[MESSAGE_SIZE+1] = {'\0'};
    char roomAux[MAX_PID_DIGITS+1] = {'\0'};
    boolean hasRead = FALSE;
	strcpy(ds, "dsr");
    strcat(ds, itoa(dsPid, roomAux));

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
    while (TRUE) {
        message_t *message;
        if((aux = read(fd, message, sizeof(message_t))) < 0){
            perror("Failed to read user name.");
            exit(0);
        }
        else if (aux > 0) {
            strcpy(message->msg, msg);
            strcpy(message->userName, userName);
            message->userPid = userPid;
            if((aux = write(SchatRoomFD, USER_MESSAGE, sizeof(USER_MESSAGE))) < 0){
                perror("Failed to write user message.");
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
    printf("Sending message...\n");
}

boolean
uniqueUser(char* userName){
	return TRUE; //HARCODEADOOOOO!
}
