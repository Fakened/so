#include <sys/wait.h>
#include <signal.h>
#include <iostream>
#include <sys/errno.h>
#include <sys/types.h> 
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <vector>
using namespace std;
vector<int> ppid;
int semId, shmId;
long numOfWriters, numOfReaders, space;
void del(){
    try {
        semctl(semId, 0, IPC_RMID);
    } catch (exception &e) {
        cerr << e.what() << endl;
        exit(-1);
    }
    try {
        shmctl(shmId, IPC_RMID, 0);
    } catch (exception &e) {
        cerr << e.what() << endl;
        exit(-1);
    }
}
void interruption(int signum)
{
    for(auto p : ppid){
        kill(p, SIGINT);
    }
    del();
    exit(signum);
}
int main(int argc, char* argv[]) {
    int i, *adress;
    char *end;
    
    void interruption(int signum);
    signal(SIGINT, interruption);
    if(argc != 4){
        cout << "Main: Nieodpowiednia liczba argumentow" << endl;
        exit(-1);
    }
    errno = 0;
    try {
        numOfWriters = strtol(argv[1], NULL, 10);
        numOfReaders = strtol(argv[2], NULL,10);
        space = strtol(argv[3], NULL, 10);
        if (errno != 0){
            throw runtime_error("Main: Blad strtol");
        }
    } catch (exception &e) {
        cerr << e.what() << endl;
        exit(-1);
    }
    if(numOfWriters <=0  || numOfReaders <= 0 || space <= 0) {
        cout << "Main: Nieprawidlowe wartosci podane w argumentach Main, powinny byc 3 argumenty o liczbach dodatnich!" << endl;
        exit(-1);
    }
    
    FILE *pd, *pd1;
    int processLimit, numOfProcess;
    try {
        pd = popen("ps ux | wc -l", "r");
    } catch (exception &e) {
        cerr << e.what() << endl;
        exit(-1);
    }
    try {
        fscanf(pd, "%d", &numOfProcess);
    } catch (exception &e) {
        cerr << e.what() << endl;
        exit(-1);
    }
    try {
        pclose(pd);
    } catch (exception &e) {
        cerr << e.what() << endl;
        exit(-1);
    }

    try {
        pd = popen("ulimit -p", "r");
    } catch (exception &e) {
        cerr << e.what() << endl;
        exit(-1);
    }
    try {
        fscanf(pd, "%d", &processLimit);
    } catch (exception &e) {
        cerr << e.what() << endl;
        exit(-1);
    }

    numOfProcess -= 4;
    try {
        pclose(pd);
    } catch (exception &e) {
        cerr << e.what() << endl;
        exit(-1);
    }

    if (numOfProcess + numOfReaders + numOfWriters > processLimit) {
        cout << "Przekroczono limit procesów" << endl;
        exit(-1);
    }

    try {
        semId = semget(997, 2, IPC_CREAT|0606);
    } catch (exception &e) {
        cerr << e.what() << endl;
        exit(-1);
    }
    for (i = 0; i < 2; i++){
        try {
            semctl(semId, i, SETVAL, 1);
        } catch (exception &e) {
            cerr << e.what() << endl;
            exit(-1);
        }
    }
    try {
        shmId = shmget(997, sizeof(int), IPC_CREAT|0606);
    } catch (exception &e) {
            cerr << e.what() << endl;
            exit(-1);
    }
    try {
        adress = (int*)shmat(shmId, 0, 0);
        adress[0] = 0;
        shmdt(adress);
    } catch (exception &e) {
            cerr << e.what() << endl;
            exit(-1);
    }

    int x;
    for(i = 0; i < numOfWriters; i++){
        switch (x = fork())
        {
        case -1:
            cerr << "Main: Blad podczas tworzenia fork() dla piszarza";    
        case 0:
            try {
                execl("pisarz","/pisarz", argv[3], NULL);
            } catch (exception &e) {
                del();
                cerr << "Main: Blad podczas tworzenia exec() dla piszarza";
                exit(-1);
            }
        default:
            ppid.push_back(x);
        }
    }

    for(i = 0; i < numOfReaders; i++) {
        switch (x = fork())
        {
        case -1:
            cerr << "Main: Blad podczas tworzenia fork() dla czytelnika";    
        case 0:
            try {
                execl("czytelnik","./czytelnik", argv[3], NULL);
            } catch (exception &e) {
                cerr << "Main: Blad podczas tworzenia exec() dla czytelnika";
                exit(-1);
            }
        default:
            ppid.push_back(x);
        }
    }

    wait(NULL);
    interruption(SIGINT);
}