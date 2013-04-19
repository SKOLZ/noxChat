#ifndef MARSHALL_H
#define MARSHALL
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>

#define MAX_BYTE_LENGTH 256
int createIPC(char*fifoName);
int getIdentifier(char* identifier, int mode);
int getInfo(int fd, void *info, int size, long priority);
int putInfo(int fd, void *info, int size, long priority);
void endIPC(int fd);
void removeIPC(void);
typedef char byte;

typedef struct msgq{
	long mtype;
	byte mtext[MAX_BYTE_LENGTH];
}msgq_t;

#include "marshall.c"
#endif
