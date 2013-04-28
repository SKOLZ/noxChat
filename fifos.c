#include "server.h"
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
