#include <stdio.h>
#include <errno.h>

int
main(int argc, char **argv) {
	if(argc != 2) {
		perror("Invalid amount of arguments");
		exit(1);
	}

	printf("\nBienvenido a la sala de chat nro. %s\n", argv[0]);
	printf("====================================\nPID: %s\n", argv[1]);
	return 0;
}
