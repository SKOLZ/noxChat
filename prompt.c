#include "server.h"
#include "prompt.h"

char userName[NAME_SIZE+1] = {'\0'};
char userPid[MAX_PID_DIGITS+1] = {'\0'};
char message[MESSAGE_SIZE+1] = {'\0'};
char roomNumber[MAX_ROOM_DIGITS+1] = {'\0'};
char roomPid[MAX_PID_DIGITS+1] = {'\0'};

int
main(int argc, char **argv) {
    if (argc != 4) {
        printf("\nInvalid amount of arguments\n");
        exit(1);
    }
    
    static struct sigaction act;
    void catchint(int);
    
    act.sa_handler = catchint;
    sigfillset(&(act.sa_mask));
    
    sigaction(SIGINT, &act, NULL);
    
    strcpy(userName, argv[0]);
    strcpy(userPid, argv[1]);
    strcpy(roomNumber, argv[2]);
    strcpy(roomPid, argv[3]);
    startPrompt();
    exit(0);
}

void
catchint(int i) {
    strcpy(message, "/quit");
    sendMessage();
}

void
startPrompt(void) {
    int i;
    boolean flag;
    char c;
    while (TRUE) {
        i = 0;
        flag = TRUE;
        while((c = getchar()) != '\n' && flag) {
            message[i++] = c;
            if (i == MESSAGE_SIZE) {
                flag = FALSE;
            }
        }
        if(!flag) {
            while(getchar() != '\n');
        }
        message[i] = '\0';
        sendMessage();
    }
}

void
sendMessage(void) {
    char ds[NAME_SIZE+1] = {'\0'};
    boolean hasRead = FALSE;
    int aux, ids;
    identifier_t id;
    
    info_t messageInfo;
    
	strcpy(ds, "dsr");
    strcat(ds, userPid);
    
    ids = createIPC(ds);
    
    
    id = getIdentifier(ds, O_RDWR, ids);
    if(id.fd == -1){
		perror("IPC open failed");
        exit(0);
	}
    
    message_t *msg = (message_t *)malloc(sizeof(message_t));
    strcpy(msg->msg, message);
    strcpy(msg->userName, userName);
    memcpy(messageInfo.mtext, msg, sizeof(message_t));
    messageInfo.mtype = atoi(userPid);
    if((aux = putInfo(id, &messageInfo, sizeof(info_t))) < 0){
        perror("Failed to write message.");
        exit(0);
    }
    free(msg);
    if (strcmp(message, "/quit") == 0) {
        exit(0);
    }
}
