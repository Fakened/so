#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <errno.h>

int semafor, key, memory;
char *adres;

static void initSem();
static void delSem();
static void setSemPlus(int semNum);
static void setSemMinus(int semNum);
static void semReset(int semNum);
static void setSem(void);
void upd();
void upa();
void detachMem();

int main()
{
    FILE *file;
    char sign;

    if (!(file = fopen("surowce", "r")))
    {
        fprintf(stderr, "file does not exist\n");
        exit(EXIT_FAILURE);
    }
    
    if(fgetc(file) == EOF) {
        printf("file is empty\n");
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_SET);

    initSem();
    setSem();
    upd();
    upa();



    do {
        setSemMinus(1);
        
        sign = fgetc(file);
        *adres = sign;
        printf("producer: %c\n", sign);

        setSemPlus(0);

    } while(sign != EOF);

    setSemMinus(1);
    


    detachMem();
    delSem();
    printf("end of producer\n");
    

    exit(0);
}

static void initSem()
{
    //key = ftok(".", 'A');
    key = 4567
    semafor = semget(key, 2, IPC_CREAT | 0200);
    if (semafor == -1)
    {

        perror("semget error");

        exit(EXIT_FAILURE);
    }
}

static void setSem(void)
{
    int ustaw_sem;
    for (int i = 0; i < 2; i++)
    {
        ustaw_sem = semctl(semafor, i, SETVAL, 0);
        if (ustaw_sem == -1)
        {
            printf("semctl error %d.\n", i);
            exit(EXIT_FAILURE);
        }
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

static void semReset(int semNum)
{
    int reset;
    reset = semctl(semafor, semNum, SETVAL, 1);
    if (reset == -1)
    {
        printf("semctl error.\n");
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

    if (zmien_sem == -1)
    {
        if (errno == EINTR){
            setSemMinus(semNum);
        }
        else {
            perror("semop error\n");
            exit(EXIT_FAILURE);
        }
    }
}

void upd()
{
    memory = shmget(key, 1, 0600 | IPC_CREAT);
    if (memory == -1)
    {
        printf("shmget error\n");
        exit(EXIT_FAILURE);
    }
}

void upa()
{
    adres = shmat(memory, 0, 0);
    if (*adres == -1)
    {
        printf("shmat error\n");
        exit(EXIT_FAILURE);
    }
}

void detachMem()
{
    int  disconnect1, disconnect2;
    disconnect1 = shmctl(memory, IPC_RMID, 0);
    disconnect2 = shmdt(adres);
    if (disconnect1 == -1 || disconnect2 == -1)
    {
        printf("shmctl or shmdt error\n");
        exit(EXIT_FAILURE);
    }
}