#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

int amount[1];

int
main(void) {
	int fd;
	system("clear");
	if((fd = open("server.cfg", O_RDONLY)) < 0) {
		printf("Server is offline. Try connecting again later...\n");
		exit(1);
	} else {
		int rd;
		read(fd, amount, (size_t)sizeof(int));
		int* pids = malloc(amount[0]*sizeof(int));
		if((rd = read(fd, pids, (size_t)amount[0]*sizeof(int))) < (size_t)amount[0]*sizeof(int)) {
			perror("Failed to read pids on server.cfg");
			close(fd);
			exit(3);
		} else {
			printf("Indique a que sala desea conectarse:\n\n");
			int i;
			for(i = 0; i < amount[0]; i++) {
				printf("Sala %d - PID: %d\n", i+1, pids[i]);
			}
		}
	}
	int opt;
	do {
		printf("\nOpcion: ");
		scanf("%d", &opt);
	}while (opt < 0 && opt > amount[0]);
	printf("Ha seleccionado la opcion %d\n", opt);
	
	exit(0);
}

