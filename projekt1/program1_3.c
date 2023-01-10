#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
int main(){
    int pid, ppid, uid, gid, i, x, asd;
	char cmd[40];
	sprintf(cmd,"pstree -p %d",getpid());
        printf("%-20s%-20s%-20s%-20s\n", "PID", "PPID", "UID", "GID");
	for(i =0; i<3; i++){
		switch (fork()){
		case -1:
			perror("fork error");
			exit(1);
		case 0:
			//x = getpid();
			if(execl("./p11", NULL) == -1){
				perror("exec error");
				exit(2);
			}
			break;
		default:
			system(cmd);
			break;
		}
	}

	for(i=0;i<3;i++){
		if((asd = wait(&x))==-1){
			perror("wait error");
			exit(3);
		}
		printf("PID potomka: %i status: %i\n", asd, x);
	}
	exit(0);
}
