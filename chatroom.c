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
	welcomeUsers(SchatRoom, RchatRoom);
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
	int fdRead, fdWrite, aux1, aux2, aux3;
    char userName[NAME_SIZE+1] = {'\0'};
    char *pid = malloc(sizeof(pid_t));
    char protocol[2] = {'\0'};
    
	/*--begining reading user name--*/
	if((fdRead = open(fifoRead, O_RDWR)) < 0){
		perror("Fifo open failed");
        exit(0);
	}
    SchatRoomFD = fdRead;
    printf("fdread = %d\n", fdRead);
	while(TRUE){
        printf("fdread = %d\n", fdRead);
        if((aux1 = read(fdRead, protocol, sizeof(USER_MESSAGE))) < 0){
			perror("Failed to read protocol.");
            exit(0);
		}
        if(!strcmp(protocol, USER_MESSAGE)) {
            message_t *message = malloc(sizeof(message_t));
            if((aux1 = read(fdRead, message, sizeof(message_t))) < 0){
                perror("Failed to read message structure.");
                exit(0);
            }
            broadcast(message);
        } else if(!strcmp(protocol, USER_CONNECTS)) {
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
                
                /*--ending reading user name--*/
                /*--begining writing user Available--*/
                if((fdWrite = open(fifoWrite, O_WRONLY)) < 0){
                    perror("Oppening name Available in fifo failed");
                }
                if(uniqueUser(userName)) {
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
}
void
addToUserList(char *userName, pid_t pid) {
    if (users == NULL) {
        users = malloc(sizeof(usrData_t));
        strcpy(users->userName, userName);
        users->userPid = pid;
    } else {
        usrData_t *curr = users;
        while (curr->next != NULL) {
            curr = curr->next;
        }
        curr->next = malloc(sizeof(usrData_t));
        strcpy(curr->next->userName, userName);
        curr->next->userPid = pid;
    }
    showUsers();
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
        printf("PID: %d\n\n", curr->userPid);
        curr = curr->next;
    }
}

void
listenToUser(char *userName, pid_t userPid, pid_t dsPid) {
    char ds[NAME_SIZE+1] = {'\0'};
    char roomAux[MAX_PID_DIGITS+1] = {'\0'};
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
    while (TRUE) {
        message_t *message = malloc(sizeof(message_t));
        if((aux = read(fd, message, sizeof(message_t))) < 0){
            perror("Failed to read user name.");
            exit(0);
        } else if (aux > 0) {
            if((aux = write(SchatRoomFD, USER_MESSAGE, sizeof(USER_MESSAGE))) < 0){
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
    char fifo[NAME_SIZE+1] = {'\0'};
    char userPid[MAX_PID_DIGITS+1] = {'\0'};
    int fd, aux;
    while (curr != NULL) {
        printf("WHILE\n");
            strcpy(fifo, "r_msg");
            strcat(fifo, itoa(curr->userPid, userPid));
            if((fd = open(fifo, O_WRONLY)) < 0){
                perror("fifo open failed");
                exit(0);
            }
            if((aux = write(fd, message, sizeof(message_t))) < 0){
                perror("Failed to write user message.");
                exit(0);
            }
            close(fd);

        curr = curr->next;
    }
    printf("SALI\n");
}

boolean
uniqueUser(char* userName){
	return TRUE; //HARCODEADOOOOO!!!!!!!!!!
}
