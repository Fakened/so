#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <limits.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/shm.h>

#define SP 0
#define SC 1
#define W1 2
#define W2 3
#define W3 4  
void makeMem(void);
void conectMem(void);
void closeMem(void);
void connectSem(void);
void semafor_p(int);
void semafor_v(int);
void handler(int sig);
int memId;
key_t memoryKey;
int semaforId;
key_t semKey;

long int *mem;

int main(int argc, char* argv[])
{
    signal(SIGINT, *handler);
    connectSem();
    makeMem();
    conectMem();
    int pid = getpid();
    int i = 0;
    int letter;
    while(1) {
        semafor_p(W2);
        mem[2] ++;
        if (mem[2] == 1){
            semafor_p(SC);
        }
        semafor_v(W2);
        semafor_p(SP);
        printf("Pisarz wszedl do biblioteki %d\n", mem[1]);
        letter = (rand()%25)+65;
        mem[3] = letter;
        semafor_v(SP);
        semafor_p(W2);
        mem[2] --;
        if (mem[2] == 0){
            semafor_v(SC);
        }
        semafor_v(W2);
    }
    closeMem();
    exit(0);
}

void makeMem()
{
    memoryKey = ftok(".", 0);
    if(memoryKey == -1) {
        perror("reader \nBlad tworzenia klucza\n");
        exit(-1);
    }
    memId = shmget(memoryKey, 3 * sizeof(long int)+1, 0600 | IPC_CREAT);
    if(memId == -1){
        perror("reader Blad tworzenia pamieci dzielonej\n");
        exit(-3);
    }

}

void conectMem()
{
    mem = shmat(memId, 0, 0);
    if(*mem == -1 && errno !=0) {
        perror("reader Problem z przydzieleniem pamieci\n");
        exit(-1);
    }
}

void closeMem()
{

    if(shmctl(memId, IPC_RMID, 0) || shmdt(mem)){
        perror("reader Blad odlaczenia pamieci dzielonej\n");
        exit(-1);
    }

}

void connectSem(void)
{
    semKey = ftok(".", 0);
    if(semKey == -1){
        perror("reader Semafor: Blad tworzenia klucza\n");
        exit(-1);
    }

    semaforId = semget(semKey, 5, IPC_CREAT | 0600);
    if(semaforId == -1){
        perror("reader Blad uzyskania dostepu do semafora\n");
        exit(-2);
    }
}

void semafor_p(int nr)
{
    int x;
    struct sembuf buforSem;
    buforSem.sem_num = nr;
    buforSem.sem_op = -1;
    buforSem.sem_flg = 0;

    x = semop(semaforId, &buforSem, 1);
    if(x == -1) {
        if(errno == 4) {
            semafor_p(nr);
        }
        else {
            perror("reader Blad zamykania semafora\n");
            exit(-1);
        }
    }
}

void semafor_v(int nr)
{
    int x;
    struct sembuf buforSem;
    buforSem.sem_num = nr;
    buforSem.sem_op = 1;
    buforSem.sem_flg = 0;

    x = semop(semaforId, &buforSem, 1);
    if(x == -1){
        if(errno == 4){
            semafor_v(nr);
        }
        else {
            perror("reader Blad otwierania semafora\n");
            exit(-1);
        }
    }
}

void handler(int sig)
{
    exit(0);
}