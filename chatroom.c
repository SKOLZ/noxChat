#include <stdio.h>
#include <errno.h>

int roomNumber;
int roomPid;
int clientNumber;
int *clientPids;

int
main(int argc, char **argv) {
	if(argc != 2) {
		perror("Invalid amount of arguments");
		exit(1);
	}

	
	roomNumber = atoi(argv[0]);
	roomPid = atoi(argv[1]);

	printf("\nChat room nbr. %d has been created with %d\n",\
	roomNumber + 1, roomPid);
	while(1);
}
