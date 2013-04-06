#include "server.h"

int roomNumber;
int roomPid;
int clientNumber;
int *clientPids;
usrData users;

int
main(int argc, char **argv) {
	if(argc != 2) {
		perror("Invalid amount of arguments");
		exit(1);
	}
	roomNumber = atoi(argv[0]);
	roomPid = atoi(argv[1]);

	int aux;
	
	char roomAux[MAX_ROOM_DIGITS];
	char SchatRoom[NAME_SIZE];
	char RchatRoom[NAME_SIZE];
	
	strcpy(SchatRoom, "SchatRoom"); 
	strcpy(RchatRoom, "RchatRoom");
	
	strcat(SchatRoom, itoa(roomNumber+1, roomAux));
	strcat(RchatRoom, itoa(roomNumber+1, roomAux));
	
	char *fifoRead = SchatRoom;
	char *fifoWrite = RchatRoom;
	
	printf("\nChat room nbr. %d has been created with %d\n",\
	roomNumber + 1, roomPid);
	/*--creating fifos--*/
	if(mkfifo(fifoRead, 0666) == -1){ 
		perror("creating fifo read error");
        exit(0);//NUEVOOOOOOOOOOOOOOOOOOOOOOOOOOO
	}
	if(mkfifo(fifoWrite, 0666) == -1){
		perror("creating fifo write error");
        exit(0);//NUEVOOOOOOOOOOOOOOOOOOOOOOOOOOO
	}
	
	while(TRUE){
		welcomeUsers(fifoRead, fifoWrite);
	}
}		
		
void
welcomeUsers(char *fifoRead, char *fifoWrite){
	int fdRead, fdWrite, aux1, aux2;
	boolean hasRead = FALSE;
    char userName[NAME_SIZE+1] = {'\0'};
    char *pid = malloc(sizeof(pid_t));

	/*--begining reading user name--*/
	if((fdRead = open(fifoRead, O_RDWR)) < 0){
		perror("fifo open failed");
        exit(0);
	}
	while(!hasRead) {
		if((aux1 = read(fdRead, userName, NAME_SIZE+1)) < 0){
			perror("Failed to read user name.");
            exit(0);
		}
        if((aux2 = read(fdRead, pid, sizeof(pid_t))) < 0){
			perror("Failed to read pid.");
            exit(0);
		}
		if(aux1 > 0 && aux2 > 0) {
			printf("User \"%s\" has joined room number %d - User PID = %s\nserver$:>", userName, roomNumber+1, pid);
            
			hasRead = TRUE;
			/*--ending reading user name--*/
			/*--begining writing user Available--*/
			if((fdWrite = open(fifoWrite, O_WRONLY)) < 0){
				perror("opening name Available in fifo failed");
			}
			if(uniqueUser(userName)){
				close(fdRead);
				addToUserList(userName, atoi(pid));
				if(write(fdWrite, "y", 2) == -1){
					perror("writing name Available failed");
				}
				/*-- creatin dedicated server for user--*/
				switch(fork()){
					case -1: {
						perror("failed to fork");
						exit(2);
						break;
					}
					case 0: {
						listenToUser();
						break;
					}
					default: {
						break;
					}
				}	
			}
			else{
				if(write(fdWrite, "n", 2) == -1){
					perror("writing name Availabl failed");
				}
			}
			close(fdWrite);
			/*ending writing user available--*/

		}
	}
}

void
addToUserList(char *userName, pid_t pid) {
    /*if (users == NULL) {
        users = (usrData *)malloc(sizeof(usrData));
        users.usrName = userName;
        users.pid = pid;
    } else {
        usrData curr = users;
        while (curr.next != NULL) {
            curr = curr.next;
        }
        curr.next = (usrData *)malloc(sizeof(usrData));
        curr.next.usrName = userName;
        curr.next.pid = pid;
    }*/
}

void
listenToUser(void) {
    exit(0);
}

boolean
uniqueUser(char* userName){
	return TRUE; //HARCODEADOOOOO!
}
