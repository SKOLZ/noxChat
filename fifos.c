#include "server.h"

int
createIPC(char*fifoName){
    return mkfifo(fifoName, 0666);
}

identifier_t
getIdentifier(char* id, int mode, int unuse){
    identifier_t ans;
    ans.fd = open(id, mode);
    strcpy(ans.address, id);
    return ans;
}

int
getInfo(identifier_t id, info_t *info, int size, long priority){
	return read(id.fd, info, size);
}

int
putInfo(identifier_t id, info_t *info, int size){
	return write(id.fd, info, size);
}

void
endIPC(identifier_t id){
	close(id.fd);
}

void
removeIPC(void){
	system("rm -rf SchatRoom*");
    system("rm -rf RchatRoom*");
    system("rm -rf dsr*");
    system("rm -rf r_msg*");
}
