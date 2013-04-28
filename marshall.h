#ifndef MARSHALL_H
#define MARSHALL
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/stat.h>
#define MAX_BYTE_LENGTH 256

typedef char byte;

typedef struct info{
	long mtype;
	byte mtext[MAX_BYTE_LENGTH];
}info_t;

typedef struct identifier {
    int fd;
    char address[32];
} identifier_t;

int createIPC(char*fifoName);
identifier_t getIdentifier(char* identifier, int mode);
int getInfo(identifier_t i, info_t *info, int size, long priority);
int putInfo(identifier_t i, info_t *info, int size);
void endIPC(identifier_t i);
void removeIPC(void);

#endif
