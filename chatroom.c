#include "server.h"

int roomNumber;
int roomPid;
int clientNumber;
int *clientPids;
char userName[NAME_SIZE+1];

int
main(int argc, char **argv) {
	if(argc != 2) {
		perror("Invalid amount of arguments");
		exit(1);
	}
	roomNumber = atoi(argv[0]);
	roomPid = atoi(argv[1]);

	int aux;
	
	char roomAux[2];
	char SchatRoom[32];
	char RchatRoom[32];
	
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
	}
	if(mkfifo(fifoWrite, 0666) == -1){
		perror("creating fifo write error");
	}
	
	while(TRUE){
		welcomeUsers(fifoRead, fifoWrite);
	}
}		
		
void
welcomeUsers( char *fifoRead, char *fifoWrite){
	int fdRead;
	int fdWrite;
	int aux;
	boolean hasRead = FALSE;
		
		
		
	/*--begining reading user name--*/
	if((fdRead = open(fifoRead, O_RDWR)) < 0){
		perror("fifo open failed");
	}
	while(!hasRead){
		if((aux = read(fdRead, userName, NAME_SIZE)) < 0){
			perror("read failed");
		}
		if(aux > 0){
			printf("A new user has joined room number %d: %s \n server$:>",roomNumber+1, userName);
			hasRead = TRUE;
			/*--ending reading user name--*/
			/*--begining writing user Available--*/
			if((fdWrite = open(fifoWrite, O_WRONLY)) < 0){
				perror("opening name Available in fifo failed");
			}
			if(uniqueUser(userName)){
				close(fdRead);
				//addToUserList(userName);
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
						//listenToUser();
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

boolean
uniqueUser(char* userName){
	return TRUE; //HARCODEADOOOOO!
}
