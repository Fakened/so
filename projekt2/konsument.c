#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <errno.h>

int semafor, key, memory, disconnect1, disconnect2;
char *adres;

static void delSem();
static void initSem();
static void setSemPlus(int semNum);
static void setSemMinus(int semNum);
static void ustaw_semafor(void);
static void semReset(int semNum);
void upd();
void upa();
void detachMem();

int main()
{
    FILE *file;

    if (!(file = fopen("produkty", "w")))
    {
        fprintf(stderr, "file does not exist\n");
        exit(EXIT_FAILURE);
    }

    initSem();
    upd();
    upa();

    
    while(1) {
        
        setSemMinus(0);

        if(*adres == EOF) {
            break;
        }

        printf("consumer: %c\n", *adres);
        fprintf(file, "%c", *adres);

        setSemPlus(1);
    };

    setSemPlus(1);

    detachMem();
    fclose(file);
    printf("end of consumer\n");

    exit(0);
}

static void initSem()
{
    //key = ftok(".", 'A');
    key = 4567;
    semafor = semget(key, 2, 0404 | IPC_CREAT);
    if (semafor == -1)
    {

        perror("semget error");

        exit(EXIT_FAILURE);
    }
}

static void setSemPlus(int semNum)
{
    int zmien_sem;
    struct sembuf bufor_sem;
    bufor_sem.sem_num = semNum;
    bufor_sem.sem_op = 1;
    bufor_sem.sem_flg = 0;

    zmien_sem = semop(semafor, &bufor_sem, 1);

    if (zmien_sem == -1)
    {
            perror("semop error\n");
            exit(EXIT_FAILURE);
    }
}

static void setSemMinus(int semNum)
{
    int zmien_sem;
    struct sembuf bufor_sem;
    bufor_sem.sem_num = semNum;
    bufor_sem.sem_op = -1;
    bufor_sem.sem_flg = 0;

    zmien_sem = semop(semafor, &bufor_sem, 1);

    if (zmien_sem == -1) {

        if (errno == EINTR) {
            setSemMinus(semNum);
        }
        else {
            printf("semop error\n");
            exit(EXIT_FAILURE);
        }
    }
}

void upd()
{
    memory = shmget(key, 0, IPC_CREAT);
    if (memory == -1)
    {
        printf("shmget error or producer has not started\n");
        delSem();
        exit(EXIT_FAILURE);
    }
}

void upa()
{
    adres = shmat(memory, 0, SHM_RDONLY);
    if (*adres == -1)
    {
        printf("shmat error\n");
        delSem();
        exit(EXIT_FAILURE);
    }
}

void detachMem()
{
    disconnect2 = shmdt(adres);
    if (disconnect2 == -1)
    {
        printf("shmdt error\n");
        exit(EXIT_FAILURE);
    }
}

static void delSem()
{
    int set;
    set = semctl(semafor, 0, IPC_RMID);

    if (set == -1)
    {
        printf("semctl error\n");

        exit(EXIT_FAILURE);
    }
}
