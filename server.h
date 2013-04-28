#ifndef SERVER_H
#define SERVER_H
    #include <stdio.h>
    #include <errno.h>
    #include <fcntl.h>
    #include <string.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <signal.h>
    #include <time.h>
    #include <sys/types.h>
    #include <sys/socket.h> 
    #include <sys/un.h> 
    
    #define TRUE 1
    #define FALSE 0
    #define MESSAGE_SIZE 140
    #define NAME_SIZE 32
    #define CHAT_ROWS 20
    #define CHAT_COLS 80
    #define MAX_ROOM_DIGITS 2
    #define MAX_PID_DIGITS 10
    #define BREAKLINE printf("\n")
    #define USER_MESSAGE '0'
    #define USER_CONNECTS '1'
    #define PRICE 1
    #define COMMAND_QUIT "/quit"
    #define COMMAND_COST "/cost"
    #define COMMAND_USERS "/users"
    #define QUEUE_KEY (key_t)0100
    typedef int boolean;       
    #include "grlfun.h"
    #include "marshall.h"
    
    typedef struct usrData {
        char userName[NAME_SIZE+1];
        pid_t userPid;
        struct usrData *next;
        time_t connectionTime;
    }usrData_t;

    typedef struct message {
        char msg[MESSAGE_SIZE+1];
        char userName[NAME_SIZE+1];
    }message_t;



    void saveData(void);
    void freeUserList(void);
    boolean isCommand(message_t *message);
    pid_t getUserPid(char *userName);
    time_t getConnectionTime(char *userName);
    void sendMessageToUser(pid_t pid, message_t *message);
    void createFifo(char *fifoName);
    void chatRoom(int serverNumber, int processID);
    void showRooms(void);
    void shutdownServer(int status);
    void welcomeUsers( char *fifoRead, char *fifoWrite);
    void listenToUser(char *userName, pid_t userPid, pid_t dsPid);
    void addToUserList(usrData_t *usrData);
    void removeFromUserList(char *userName);
    void checkCommand(char *command);
    void broadcast(message_t *message);
    void showUsers(void);
    
#endif    
    
    
