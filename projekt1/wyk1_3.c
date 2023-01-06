#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
int main(){
	int pid, ppid, uid, gid;
	pid = getpid();
	ppid = getppid();
	uid = getuid();
	gid = getgid();
	printf("%-20i%-20i%-20i%-20i\n", pid, ppid, uid, gid);
	exit(0);
}
