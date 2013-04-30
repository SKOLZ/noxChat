#include "server.h"

int
initmutex(int semkey) {
    int semid;
    
    if ( (semid = semget(semkey, 1, 0)) >= 0 ){
        return;
    }	
    if ( (semid = semget(semkey, 1, IPC_CREAT|0666)) == -1 ){
        perror("semget");
        exit(1);
    }
    semctl(semid, 0, SETVAL, 1);
    return semid;
}

void
enter(int semid) {
    struct sembuf sb;
    
    sb.sem_num = 0;
    sb.sem_op = -1;
    sb.sem_flg = SEM_UNDO;
    semop(semid, &sb, 1);
}

void
leave(int semid) {
    struct sembuf sb;
    
    sb.sem_num = 0;
    sb.sem_op = 1;
    sb.sem_flg = SEM_UNDO;
    semop(semid, &sb, 1);
}

int
createIPC(char* strKey) {
}

identifier_t
getIdentifier(char* path, int mode, int unused) {
    identifier_t ans;
    info_t trash;
    trash.mtype = 0;
    trash.mtext[0]= '\0';
    ans.fd = open(path, mode | O_CREAT, 0666);
    write(ans.fd, &trash, sizeof(info_t));
    lseek(ans.fd, 0, SEEK_SET);
    return ans;
}

int
getInfo(identifier_t id, info_t* info, int size, long priority) {
    int semid;
    boolean correct = FALSE;
    info_t trash;
    trash.mtype = 0;
    trash.mtext[0]= '\0';
    
    semid = initmutex(priority);
    while(!correct){
        enter(semid);
        read(id.fd, info, sizeof(info_t));
        lseek(id.fd, 0, SEEK_SET);
        if(info->mtype == priority){
            write(id.fd, &trash, sizeof(info_t));
            lseek(id.fd, 0, SEEK_SET);
            correct = TRUE;
        } 
        leave(semid);
    }	
    return 1;
}

int
putInfo(identifier_t id, info_t* info, int size) { 
    int semid;
    boolean correct = FALSE;
    info_t trash;
    trash.mtype = 0;
    trash.mtext[0]= '\0';
    
    semid = initmutex(info->mtype);
    
    while(!correct){
        enter(semid);
        read(id.fd, &trash, sizeof(info_t));
        lseek(id.fd, 0, SEEK_SET);
        if(trash.mtype != info->mtype){
            write(id.fd, info, sizeof(info_t));
            correct = TRUE;
            lseek(id.fd, 0, SEEK_SET);
        }
        leave(semid);
    }
    return 1;
}

void
endIPC(identifier_t id) {
    close(id.fd);
}

void
removeIPC(void) {
    system("rm -rf SchatRoom*");
    system("rm -rf RchatRoom*");
    system("rm -rf dsr*");
    system("rm -rf r_msg*");
}