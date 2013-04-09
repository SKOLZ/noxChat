#include <stdio.h>
#include "server.h"
#include "prompt.h"

char userName[NAME_SIZE+1] = {'\0'};
pid_t pid;
char message[MESSAGE_SIZE+1] = {'\0'};
char roomNumber[MAX_ROOM_DIGITS+1] = {'\0'};
char roomPid[MAX_PID_DIGITS+1] = {'\0'};

int
main(int argc, char **argv) {
    if (argc != 4) {
        printf("\nInvalid amount of arguments\n");
        exit(1);
    }
    strcpy(userName, argv[0]);
    pid = (pid_t)atoi(argv[1]);
    strcpy(roomNumber, argv[2]);
    strcpy(roomPid, argv[3]);
    startPrompt();
    exit(0);
}

void
startPrompt(void) {
    int i = 0;
    boolean flag = TRUE;
    char c;
    while (TRUE) {
        printf("%s: ", userName);
        while((c = getchar()) != '\n' && flag) {
            message[i++] = c;
            if (i == NAME_SIZE) {
                flag = FALSE;
            }
        }
        if(!flag) {
            while(getchar() != '\n');
        }
        sendMessage();
    }
}

void
sendMessage(void) {
    char ds[NAME_SIZE] = {'\0'};
    boolean hasRead = FALSE;
	strcpy(ds, "dsr");
    strcat(ds, roomPid);

	/*--creating fifos--*/
    int fd, aux;
    if((fd = open(ds, O_RDWR)) < 0){
		perror("fifo open failed");
        exit(0);
	}
    
    message_t *msg = malloc(sizeof(message_t));
    strcpy(msg->msg, message);
    strcpy(msg->userName, userName);
    msg->userPid = pid;
    if((aux = write(fd, msg, sizeof(message_t))) < 0){
        perror("Failed to write message.");
        exit(0);
    }
}
