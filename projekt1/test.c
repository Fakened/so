#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

void printIds() {
	printf("PID = %d\n", getpid());
	printf("PPID = %d\n", getppid());
	printf("UID = %d\n", getuid());
	printf("GID = %d\n", getgid());
}

int main() {
	int i;
	char cmd[256];

	sprintf(cmd, "pstree -ph %d", getpid());

	printf("Proces macierzysty: \n");
	printIds();

	for(i = 0; i < 1; i++) {
		switch(fork()) {
			case -1:
				perror("Fork error");
                exit(1);
			case 0:
				printf("Proces potomny: %d\n", i + 1);
				printIds();
				break;
			default:
				break;
		}
	}

	system(cmd);
	exit(0);
}
