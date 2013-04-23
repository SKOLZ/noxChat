#ifndef MARSHALL_H
#define MARSHALL
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>

#define MAX_BYTE_LENGTH 256

typedef char byte;

typedef struct info{
	long mtype;
	byte mtext[MAX_BYTE_LENGTH];
}info_t;

int createIPC(char*fifoName);
int getIdentifier(char* identifier, int mode);
int getInfo(int fd, info_t *info, int size, long priority);
int putInfo(int fd, info_t *info, int size);
void endIPC(int fd);
void removeIPC(void);


#include "marshall.c"
#endif
