#include "server.h"

int roomNumber;
int roomPid;
int clientNumber;
int *clientPids;
char userName[NAME_SIZE+1];

int
main(int argc, char **argv) {
	char roomAux[2];
	if(argc != 2) {
		perror("Invalid amount of arguments");
		exit(1);
	}
	
	boolean hasRead = FALSE;
	char *fifoRead = strcat("SchatRoom", itoa(roomNumber, roomAux));
	char *fifoWrite = strcat("RchatRoom", itoa(roomNumber, roomAux));
	roomNumber = atoi(argv[0]);
	roomPid = atoi(argv[1]);
	int fdRead, fdWrite;
	int aux;
	

	printf("\nChat room nbr. %d has been created with %d\n",\
	roomNumber + 1, roomPid);
	/*--creating fifos--*/
	mkfifo(fifoRead, 0666);
	mkfifo(fifoWrite, 0666);
	/*--begining reading user name--*/
	if((fdRead = open(fifoRead, O_RDWR) < 0)){
		perror("fifo open failed");
	}
	while(!hasRead){
		if((aux = read(fdRead, userName, NAME_SIZE)) < 0){
			perror("read failed");
		}
		if(aux > 0){
			printf("user name: %s\n", userName);//para ver si el chatroom leyo bien el nombre, sacar luego
			hasRead = TRUE;
			/*--ending reading user name--*/
			/*--begining writing user Available--*/
			if((fdWrite = open(fifoWrite, O_WRONLY)) < 0){
				perror("opening name Available in fifo failed");
			}
			if(uniqueUser(userName)){
				if(write(fdWrite, "y", 2) == -1){
					perror("writing name Available failed");
				}
			}
			else{
				if(write(fdWrite, "n", 2) == -1){
					perror("writing name Availabl failed");
				}
			}
			/*ending writing user available--*/
		}
	}
}

boolean
uniqueUser(char* userName){
	return TRUE; //HARCODEADOOOOO!
}
