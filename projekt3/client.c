#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <pthread.h>

#define MAX 4088 // max 8184 max na linuxie 4095

int key, queueId;
int countMessages = 0;
int flag = 0;
pthread_t senderThread, receiverThread;

typedef struct message {
    long receiver; // odbiorca
    long sender;  // nadawca
    char message[MAX];
} msgData;

void *sender();
void *receiver();
void interruption();

void attachToQueue() {
    // key = ftok(".", 'A');
    key = 1101;
    if (key == -1) {
        perror("Blad ftok\n");
        exit(-1);
    } else {
        printf("Klucz do kolejki wiadomosci zostal utowrzony\n");
    }

    queueId = msgget(key, 0606);
    if (queueId == -1) {
        perror("Blad msgget lub serwer nie dziala\n");
        exit(-1);
    } else {
        printf("Dolaczono do kolejki wiadomosci: %d\n", queueId);
    }
}

void setSignalsHandler() {
    signal(SIGINT, interruption);
}

void createThreads() {
    if (pthread_create(&senderThread, NULL, sender, NULL) == -1) {
        perror("Blad pthread_create\n");
        exit(-1);
    }

    if (pthread_create(&receiverThread, NULL, receiver, NULL) == -1) {
        perror("Blad pthread_create\n");
        exit(-1);
    }
}

void activateThreads() {
    if (pthread_join(senderThread, NULL) == -1) {
        perror("Blad pthread_join\n");
        exit(-1);
    }

    if (pthread_join(receiverThread, NULL) == -1) {
        perror("Blad pthread_join\n");
        exit(-1);
    }
}

void *sender() {
    struct msqid_ds msg;
    msgData message;
    char character;
    int max, i;
    max = 0;

    msgctl(queueId, IPC_STAT, &msg);
    max = msg.msg_qbytes - 2*(sizeof(message) - sizeof(long)); 

    while (1) {
        msgctl(queueId, IPC_STAT, &msg);
        flag = 0;

        printf("Ilosc wiadomosci w kolejce = %d\n", msg.msg_qnum);

        while (flag == 0) {
            i = 0;
            printf("\nPodaj wiadomosc: ");

            while ((character = fgetc(stdin)) != '\n') {
                if (i < (MAX - 1)) {
                    message.message[i] = character;
                }
                i++;
            }

            if (i < MAX) {
                flag = 1;
                message.message[i] = '\0';
            } else {
                printf("Przekroczono maksymalna wielkosc wiadomosci\n");
            }
            
        }
        msgctl(queueId, IPC_STAT, &msg);
        printf("\n%i  %i\n", msg.__msg_cbytes, max);
        if (msg.__msg_cbytes + 4096 > max) { //strlen(message.message) + sizeof(long) + 1
            printf("Kolejka jest pelna, wiadomosc nie zostaje wyslana oraz nastepuje zakonczenie watku wysylajacego\n");
            interruption();
            }

        message.receiver = 1;
        message.sender = getpid();

        if (msgsnd(queueId, &message, 4096, 0) == -1) { //strlen(message.message) + sizeof(long) + 1
            if (errno == EIDRM || errno == EINVAL) {
                printf("\nSerwer zakonczyl dzialanie, wiadomosc nie zostala wyslana\n");
                exit(0);
            }
            perror("Blad msgsnd\n");
            exit(-1);
        }

        countMessages++;

        printf("Wiadomosc zostala wyslana\n");
        usleep(600);
    }
}

void *receiver() {
    msgData message;

    while (1){
        if (countMessages > 0) {

            if (msgrcv(queueId, &message, MAX + sizeof(long), getpid(), 0) == -1) {
                if (errno == EIDRM || errno == EINVAL) {
                    printf("\nSerwer zakonczyl dzialanie\n");
                    exit(0);
                }
                perror("Blad msgrcv\n");
                exit(-1);
            }

            printf("\nOdebralem wiadomosc: %s\n", message.message);
            countMessages--;
            printf("Pozostalo %i wiadomosci do odebrania\n", countMessages);
            usleep(600);
        }
    }
}

void interruption() {
    if (countMessages == 0) {
        exit(0);
    }
    else {
        printf("\nNie odczytalem jeszcze wszystkich wyslanych wiadomosci\n");
        if (pthread_detach(senderThread) == -1) {
            perror("\nBlad pthread_cancel\n");
            exit(-1);
        }
        while (countMessages != 0)
            ;
        exit(0);
    }
}


int main() {
    attachToQueue();
    setSignalsHandler();
    createThreads();
    activateThreads();
}