#include <signal.h>
#include <iostream>
#include <sys/errno.h>
#include <sys/types.h> 
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/sem.h>
#include <time.h>
#include <cstdlib>
#include <fstream>
using namespace std;

int semId, shmId, *adress;
static void semMinus(int semNum);
static void semPlus(int semNum);
void interruption(int signum)
{
    exit(0);
}
int main(int argc, char* argv[]){
    void interruption(int signum);
    signal(SIGINT, interruption);
    int space;
    try {
        space = strtol(argv[1], NULL, 10);
        if (errno != 0){
            throw runtime_error("Reader: Blad strtol");
        }
    } catch (exception &e) {
        cerr << e.what() << endl;
        exit(-1);
    }
    
    int pid = getpid();
    srand(time(NULL) + pid);
    try {
        semget(997, 2, IPC_CREAT|0606);
    } catch (exception &e) {
        cerr << e.what() << endl;
        exit(-1);
    }
    try {
        shmId = shmget(997, sizeof(int), IPC_CREAT|0606);
    } catch (exception &e) {
            cerr << e.what() << endl;
            exit(-1);
    }
    try {
        adress = (int*)shmat(shmId, 0, 0);
    } catch (exception &e) {
            cerr << e.what() << endl;
            exit(-1);
    }

    string message;

    while(1){
        semMinus(1);
        if(adress[0] < space) {
            adress[0] += 1;
            if(adress[0] >= 1){
                semMinus(0);
            }
            cout << "Reader: " << pid << "come to library, readers in library: " << adress[0] << endl;
            semPlus(1);
            fstream in;
            in.open("library.txt", ios::in);
            getline(in, message);
            in.close();
            sleep(1);
            semMinus(1);
            adress[0] -= 1;
            cout << "Reader: " << pid << "out of library, readers in library: " << adress[0] << endl;
            sleep(1);
            if (adress[0] == 0){
                semPlus(0);
            }
        }
        semPlus(1);
        // sleep(1);
    }

}

static void semMinus(int semNum)
{
    struct sembuf bufor_sem;
    bufor_sem.sem_num = semNum;
    bufor_sem.sem_op = -1;
    bufor_sem.sem_flg = 0;
    errno = 0;
    try {
        semop(semId, &bufor_sem, 1);
        if (errno == EINTR) {
            semMinus(semNum);
        }
    } catch (exception &e) {
        cerr << e.what() << endl;
        exit(-1);
    }
}

static void semPlus(int semNum)
{
    struct sembuf bufor_sem;
    bufor_sem.sem_num = semNum;
    bufor_sem.sem_op = 1;
    bufor_sem.sem_flg = 0;
    errno = 0;
    try {
        semop(semId, &bufor_sem, 1);
        if (errno == EINTR) {
            semMinus(semNum);
        }
    } catch (exception &e) {
        cerr << e.what() << endl;
        exit(-1);
    }
}