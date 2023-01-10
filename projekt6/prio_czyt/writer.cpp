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
void semPlus(int semNum);
void semMinus(int semNum);
void interruption(int signum)
{
    exit(0);
}
int semId, shmId;
int main(int argc, char* argv[]){
    int  *adress;
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
    string message;
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
    message += "Writer: ";
    message += to_string(pid);
    cout << message << endl;
    while(1){
        semMinus(0);
        if(adress[0] == 0){
            adress[0] += space;
            cout << "Writer: " << pid << "come to library" << endl;
            fstream out;
            out.open("library.txt", ios::out);
            out.write((char*)&message[0], message.size());
            out.close();
            sleep(1);
            cout << "Writer: " << pid << "out of library" << endl;
            adress[0] = 0;
            cout << adress[0] << endl;
        }
        semPlus(0);
        // sleep(1);
    }

}

void semMinus(int semNum)
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

void semPlus(int semNum)
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