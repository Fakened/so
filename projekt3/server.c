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

#define MAX 4088 //max 8184

int key, queueId;
int flag = 0;

typedef struct message {
    long receiver; // odbiorca
    long sender;  // nadawca
    char message[MAX];
} msgData;

void interruption();
void enlarge();

void createMessageQueue() {
    //key = ftok(".", 'A');
    key = 1101;
    if (key == -1) {
        perror("Blad ftok\n");
        exit(-1);
    } else {
        printf("Klucz do kolejki wiadomosci zostal utworzony\n");
    }

    queueId = msgget(key, 0606 | IPC_CREAT);
    if (queueId == -1) {
        perror("Blad msgget\n");
        exit(-1);
    } else {
        printf("Utworzono kolejke wiadomosci %d\n", queueId);
    }
}

void setSignalsHandler() {
    signal(SIGINT, interruption);
    signal(SIGTERM, interruption);
}

void receiveAndSend() {
    msgData message;
    int status = 0;
    while (1) {
        printf("\nOczekiwanie na wiadomosc\n");

        status = msgrcv(queueId, &message, MAX + sizeof(long), 1, 0);
        if (status == -1) {
            if (errno == E2BIG) {
                printf("\nWielkosc wiadomosci nie jest poprawna\n");
                flag = 1;
                interruption();
            }
            perror("Blad msgrcv\n");
            flag = 1;
            interruption();
        }

        printf("Odebralem wiadomosc: %s\tod %ld\n", message.message, message.sender);

        message.receiver = message.sender;
        message.sender = 1;
        enlarge(message.message, sizeof(message.message));

        status = msgsnd(queueId, &message, 4096, 0); //strlen(message.message) + sizeof(long) + 1

        if (status == -1) {
            perror("Blad msgrcv\n");
            flag = 1;
            interruption();
        }

        printf("Odsylam wiadomosc do %ld\n", message.receiver);
        sleep(1);
    }
}

void interruption() {
    int shutDown = msgctl(queueId, IPC_RMID, NULL);
    if (shutDown == -1) {
        perror("Blad msgctl\n");
        exit(-1);
    }
    printf("\nUsunalem kolejke %d\nSerwer konczy dzialanie\n", queueId);

    if (flag == 0) {
        exit(0);
    }
    else {
        exit(-1);
    }
}

void enlarge(char *base, int size) {
    if (size) {
        for (int i = 0; i < size; i++) {
            base[i] = toupper(base[i]);
        }
    }
}

int main() {
    createMessageQueue();
    setSignalsHandler();
    receiveAndSend();
}