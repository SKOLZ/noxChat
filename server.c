#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include "server.h"

pid_t selfPid;
pid_t *pids;
int rooms;

int
main(int argc, char **argv) {

	rooms = atoi(argv[1]);
	selfPid = getpid();
	pids = malloc(rooms*sizeof(pid_t));
	system("clear");
	printf("\nNoxChat server\n==============\nProcess number %d\nRooms:\
	 %d\n", selfPid, rooms);

	if(argc != 2) {
		perror("Invalid amount of arguments");
		exit(1);
	}

	int i;
	pid_t childpid;

	for(i = 0; i < rooms ; i++) {
		pids[i] = fork();
		if(pids[i] == -1) {
		perror("Failed to Fork");
		exit(2);
		}
		if(pids[i] == 0) {
			chatRoom(i, getpid());
		}
	}
	showRooms();
	printf("Saving server data...\n");
	int fd;
	if((fd = open("server.cfg", O_RDWR | O_CREAT | O_TRUNC, 0666)) < 0) {
		perror("Failed to open server.cfg");
		shutdown(3);
	}
	int wr;
	int bytes[1] = {rooms};
	if((wr = write(fd, bytes, sizeof(int))) < sizeof(int)) {
		perror("Failed to write byte info on server.cfg");
		close(fd);
		shutdown(4);
	}
	if((wr = write(fd, pids, rooms*sizeof(pid_t))) < rooms*sizeof(pid_t)) {
		perror("Failed to write pids on server.cfg");
		close(fd);
		shutdown(5);
	}
	while(1);
	shutdown(0);
}

void
chatRoom(int number, int pid) {
	char par1[6];
	char par2[6];
	execl("chatroom", itoa(number, par1), itoa(pid, par2), NULL);
}

char*
itoa(int n, char s[]) {
	int i, sign;

	if ((sign = n) < 0)  /* record sign */
		n = -n;          /* make n positive */

	i = 0;
	do {	/* generate digits in reverse order */
		s[i++] = n % 10 + '0';   /* get next digit */
	} while ((n /= 10) > 0);     /* delete it */
	if (sign < 0)
		s[i++] = '-';
	s[i] = '\0';
	reverse(s);
	return s;
}

void
reverse(char s[]) {
	int i, j;
	char c;

	for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
		c = s[i];
		s[i] = s[j];
		s[j] = c;
	}
}

void
showRooms() {
	int i;
	printf("\nRooms: %d\n", rooms);
	for(i = 0; i < rooms; i++) {
		printf("Room name: %d - Room PID: %d\n", i, pids[i]);
	}
}

void
shutdown(int status) {
	printf("\nFreeing memory...");
	free(pids);
	printf("Done...\n");
	printf("Erasing server data...");
	remove("server.cfg");
	printf("Done...\n");
	printf("Exited with error status: %d\n", status);
	exit(status);
}
