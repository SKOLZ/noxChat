typedef int boolean;

#define TRUE 1;
#define FALSE 0;
#define MESSAGE_SIZE 140
#define NAME_SIZE 32
#define CHAT_ROWS 20
#define CHAT_COLS 80
#define MAX_ROOM_DIGITS 2
#define BREAKLINE printf("\n")

#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "grlfun.h"
#include "client.h"

void saveData(void);
void chatRoom(int serverNumber, int processID);
void showRooms(void);
void shutdown(int status);

