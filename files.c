#include "server.h"
int
initmutex(int semkey)
{
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
enter(int semid)
{
	struct sembuf sb;
	
	sb.sem_num = 0;
	sb.sem_op = -1;
	sb.sem_flg = SEM_UNDO;
	semop(semid, &sb, 1);
}

void
leave(int semid)
{
	struct sembuf sb;
	
	sb.sem_num = 0;
	sb.sem_op = 1;
	sb.sem_flg = SEM_UNDO;
	semop(semid, &sb, 1);
}
int
createIPC(char* strKey){
}

int
getIdentifier(char* path, int mode){
	return open(path, mode | O_CREAT, 0666);
}

int
getInfo(int fd, info_t* info, int size, long priority){
	int aux;
	int semid;
	boolean correct = FALSE;
	
	//semid = initmutex(priority);
		
	while(!correct){
		//enter(semid);
		info_t trash;
		
		aux = read(fd, info, sizeof(info_t));
		if(info->mtype == priority){
			correct = TRUE;
			lseek(fd, 0, SEEK_SET);
			write(fd, &info, sizeof(info_t));
		}
		lseek(fd, 0, SEEK_SET);
		//leave(semid);
		usleep(2000);
	}
	return aux;
}

int
putInfo(int fd, info_t* info, int size){ 
	int semid;
	int aux;
	
	//semid = initmutex(info->mtype);
	//enter(semid);
	aux = write(fd, info, sizeof(info_t));
	//leave(semid);
	usleep(2000);
	return aux; 
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

