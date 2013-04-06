#ifndef SERVER_H
#define SERVER_H
    #include <stdio.h>
    #include <errno.h>
    #include <fcntl.h>
    #include <string.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <signal.h>
    #include "grlfun.h"

    #define TRUE 1
    #define FALSE 0
    #define MESSAGE_SIZE 140
    #define NAME_SIZE 32
    #define CHAT_ROWS 20
    #define CHAT_COLS 80
    #define MAX_ROOM_DIGITS 2
    #define MAX_PID_DIGITS 6
    #define BREAKLINE printf("\n")

    typedef struct {
        char usrName[NAME_SIZE];
        pid_t pid;
        struct usrData *next;
    }usrData;

    typedef int boolean;

    void saveData(void);
    void chatRoom(int serverNumber, int processID);
    void showRooms(void);
    void shutdown(int status);
    void welcomeUsers( char *fifoRead, char *fifoWrite);
    void listenToUser(void);
    void addToUserList(char *userName, pid_t pid);
    void checkCommand(char *command);
#endif