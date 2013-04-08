#include <stdio.h>
#include "server.h"
#include "prompt.h"

char userName[NAME_SIZE+1] = {'\0'};
pid_t pid;
char message[MESSAGE_SIZE+1] = {'\0'};

int
main(int argc, char **argv) {
    if (argc != 2) {
        printf("\nInvalid amount of arguments\n");
        exit(1);
    }
    assignUserName(argv[0]);
    pid = (pid_t)atoi(argv[1]);
    startPrompt();
    exit(0);
}

void
assignUserName(char *name) {
    int i = 0;
    while (i < NAME_SIZE && name[i] != '\0') {
        userName[i] = name[i];
        i++;
    }
}

void
startPrompt(void) {
    int i = 0;
    boolean flag = TRUE;
    char c;
    while (TRUE) {
        printf("ENTRO\n");
        printf("%s: ", userName);
        while((c = getchar()) != '\n' && flag) {
            message[i++] = c;
            if (i == NAME_SIZE) {
                flag = FALSE;
            }
        }
        if(!flag) {
            while(getchar() != '\n');
        }
        sendMessage();
    }
    printf("SALI\n");
}

void
sendMessage(void) {
    printf("MANDO\n");
}
