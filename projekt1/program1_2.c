#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
int main(){
        int pid, ppid, uid, gid, i;
	char cmd[40];
	sprintf(cmd,"pstree -p %d",getpid());
        printf("%-20s%-20s%-20s%-20s\n", "PID", "PPID", "UID", "GID");	
	for(i =0; i<3; i++){
		if(fork()==0){
			pid = getpid();
       			ppid = getppid();
        		uid = getuid();
        		gid = getgid();
			printf("%-20i%-20i%-20i%-20i\n", pid, ppid, uid, gid);

		}
	}
	system(cmd);
}
