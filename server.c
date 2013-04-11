#include "server.h"


pid_t selfPid;
pid_t *pids;
int rooms;

int
main(int argc, char **argv) {

    static struct sigaction act;
    void catchint(int);
    
    act.sa_handler = catchint;
    sigfillset(&(act.sa_mask));
    
    sigaction(SIGINT, &act, NULL);
    
	system("clear");
	if(argc != 2) {
        printf("FATAL: Invalid amount of arguments\n");
        return 1;
    }
	rooms = atoi(argv[1]);
	selfPid = getpid();
	if((pids = (pid_t *)malloc(rooms*sizeof(pid_t))) == NULL){
		perror("Not enough memory");
	}
	printf("NoxChat server\n==============\nProcess number %d\nRooms:\
	 %d\n", selfPid, rooms);
	int i;
	pid_t childpid;

	for(i = 0; i < rooms ; i++) {
		switch(pids[i] = fork()){
		case -1: {
			perror("Failed to Fork");
			exit(2);
		}
		case 0:
			usleep(i*1000);
			chatRoom(i, getpid());
			break;
		default:
			break;
		}
	}
	showRooms();
	saveData();
	char command[NAME_SIZE+1];
	usleep(20 *1000);
    char c;
	while(TRUE) {
        i = 0;
		printf("server$:>");
		while ((c = getchar()) != '\n' && i < NAME_SIZE) {
            command[i++] = c;
        }
        checkCommand(command);
	}
}

void
checkCommand(char *command) {
    if(!strcmp(command, "/quit")) {
        shutdown(0);
    }
}

void catchint(int signo) {
    printf("FATAL: Server execution has been terminated suddenly\n");
    shutdown(0);
}

void
saveData(void) {
	BREAKLINE;
	printf("Saving server data...\n");
	int fd;
	if((fd = open("server.cfg", O_RDWR | O_CREAT | O_TRUNC, 0666)) < 0)\
	{
		perror("Failed to open server.cfg");
		shutdown(3);
	}
	int wr;
	if((wr = write(fd, &rooms, sizeof(int))) < 0) {
		perror("Failed to write byte info on server.cfg");
		close(fd);
		shutdown(4);
	}
	if((wr = write(fd, pids, rooms*sizeof(pid_t))) < 0) {
		perror("Failed to write pids on server.cfg");
		close(fd);
		shutdown(5);
	}
}

void
chatRoom(int number, int pid) {
	char par1[6];
	char par2[6];
	execl("chatroom", itoa(number, par1), itoa(pid, par2), NULL);
}

void
showRooms(void) {
	int i;
	BREAKLINE; 
	for(i = 0; i < rooms; i++) {
		printf("Room name: %d - Room PID: %d\n", i + 1, pids[i]);
	}
}

void
shutdown(int status) {
	int i;
	char roomAux[MAX_ROOM_DIGITS+1] = {'\0'};
	char SchatRoom[NAME_SIZE+1] = {'\0'};
	char RchatRoom[NAME_SIZE+1] = {'\0'};
	char ds[NAME_SIZE+1] = {'\0'};
    
	BREAKLINE;
	printf("Freeing memory...");
	
	free(pids);
	
	printf("Done...\n");
	printf("Erasing server data...");
	
	remove("server.cfg");
	for(i = 0 ; i<rooms ; i++){
		strcpy(SchatRoom, "SchatRoom"); 
		strcpy(RchatRoom, "RchatRoom");
		strcat(SchatRoom, itoa(i+1, roomAux));
		strcat(RchatRoom, itoa(i+1, roomAux));
		strcpy(ds, "ds");
        strcat(ds, itoa(i+1, roomAux));
        
		remove(RchatRoom);
		remove(SchatRoom);
        remove(ds);
	}
    system("rm -rf dsr*");
    system("rm -rf r_msg*");
	
	printf("Done...\n");
	printf("Exited with error status: %d\n", status);
	exit(status);
}
