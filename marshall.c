/*-----FUNCTIONS FOR FIFO'S-----*/
/*
int
createIPC(char*fifoName){
	return mkfifo(fifoName, 0666);
}

int
getIdentifier(char* identifier, int mode){
	return open(identifier, mode);
}

int
getInfo(int fd, info_t *info, int size, long priority){
	return read(fd, info, size);
}

int
putInfo(int fd, info_t *info, int size){
	return write(fd, info, size);
}

void
endIPC(int fd){
	close(fd);
}

void
removeIPC(void){
	system("rm -rf SchatRoom*");
    system("rm -rf RchatRoom*");
    system("rm -rf dsr*");
    system("rm -rf r_msg*");
}
*/
/*----- FUNCTIONS FOR MESSAGE QUEUE'S -----*/

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

