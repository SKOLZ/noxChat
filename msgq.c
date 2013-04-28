#include "server.h"
int
createIPC(char* strKey){
}

int
getIdentifier(char* strKey, int mode){
	return msgget(QUEUE_KEY, 0666 | IPC_CREAT);
}

int
getInfo(int qid, info_t* info, int size, long priority){
	int aux;
	aux = msgrcv(qid, info, MAX_BYTE_LENGTH, priority, MSG_NOERROR);
	return aux;
}

int
putInfo(int qid, info_t* info, int size){ 
	return msgsnd(qid, info, MAX_BYTE_LENGTH, MSG_NOERROR) == 0 ? 1 : -1;
}

void
endIPC(int fd){
}

void
removeIPC(void){
}
