#include "server.h"
int
createIPC(char* strKey){
}

identifier_t
getIdentifier(char* strKey, int mode, int unused){
    identifier_t ans;
    ans.fd = msgget(QUEUE_KEY, 0666 | IPC_CREAT);
    strcpy(ans.address, strKey);
	return ans;
}

int
getInfo(identifier_t id, info_t* info, int size, long priority){
	int aux;
	aux = msgrcv(id.fd, info, MAX_BYTE_LENGTH, priority, MSG_NOERROR);
	return aux;
}

int
putInfo(identifier_t id, info_t* info, int size){ 
	return msgsnd(id.fd, info, MAX_BYTE_LENGTH, MSG_NOERROR) == 0 ? 1 : -1;
}

void
endIPC(identifier_t id){
}

void
removeIPC(void){
}
