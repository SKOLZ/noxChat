#include "server.h"

int
createIPC(char* strKey) {
    return 1;
}

identifier_t
getIdentifier(char* strKey, int mode){
    struct sockaddr_un sockaddr;
    identifier_t ans;
    strcpy(ans.address, strKey);
    
    sockaddr.sun_family=AF_UNIX;
	strcpy(sockaddr.sun_path, strKey);
    
    if((ans.fd = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1) {
        perror("failed to socket call");
        exit(1);
    }
    if ((bind(ans.fd, (struct sockaddr *) &sockaddr, sizeof(struct sockaddr_un)) == -1)) {
		close(ans.fd);
		perror("bind call failed");
		exit(1);
	}
}

int
getInfo(identifier_t id, info_t* info, int size, long priority) {
    printf("id.fd = %d\n", id.fd);
    if (recv(id.fd, info, size, 0) == -1) {
        perror("recv call failed");
        exit(1);
    }
    return size;
}

int
putInfo(identifier_t id, info_t* info, int size){     
    struct sockaddr_un sockaddr;
    
    sockaddr.sun_family = AF_UNIX;
    strcpy(sockaddr.sun_path, id.address);
    
    if (sendto(id.fd, info, size, 0, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) == -1) {
        perror("sendto call failed");
        exit(1);
    }
}

void
endIPC(identifier_t id){
    unlink(id.address);
    close(id.fd);
}

void
removeIPC(void){
    system("rm -rf SchatRoom*");
    system("rm -rf RchatRoom*");
    system("rm -rf dsr*");
    system("rm -rf r_msg*");
}