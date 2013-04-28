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
generateKey(char* string){
	int value;
    sscanf(string,"%*[^0-9]%d",&value);
    switch(string[0]){
		case 'R': {
			return value*3000;
			break;
		}
		case 'S': {
			return value*1000;
			break;
		}
		case 'r': {
			return value*2;
			break;
		}
		case 'd': {
			return value;
			break;
		}
	}	
}

int
createIPC(char* strKey){
}

int
getIdentifier(char* strKey, int mode){
	void* mem;
	int memid;
	int key;
	key = generateKey(strKey);
	if ( (memid = shmget(key, sizeof(info_t), IPC_CREAT|0666)) == -1 ){
		perror("shmget");
		exit(1);
	}
	if ( !(mem = shmat(memid, NULL, 0)) ){
		perror("shmat");
		exit(1);
	}
	memset(mem, 0, sizeof(info_t));
	return (int)mem;
}	

int
getInfo(int addr, info_t* info, int size, long priority){
	int semid;
	boolean correct = FALSE;
	
	semid = initmutex(priority);
	while(!correct){
		enter(semid);
		memcpy(info, (void*)addr, sizeof(info_t));
		if(info->mtype == priority){
			memset((void*)addr, 0, sizeof(info_t));
			correct = TRUE;
		}
		leave(semid);
		usleep(2000);
	}
	
	return 1;
}

int
putInfo(int mem, info_t* info, int size){ 
	int semid;
	semid = initmutex(info->mtype);
	enter(semid);
	memcpy((void*)mem, info, sizeof(info_t));
	leave(semid);
	usleep(2000);
	return 1;
}

void
endIPC(int mem){	
	shmdt((void*)mem);
}

void
removeIPC(void){
	system("sh kill_IPCS.sh");
}
