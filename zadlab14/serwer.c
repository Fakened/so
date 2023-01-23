#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <signal.h>
#define MAX 15

static void blad(char opis[])
{
    printf("%s/n", opis);
    exit(EXIT_FAILURE);
}

void interruption(int signum)
{
    system("rm gniazdo_serwera");
    exit(0);
}

int main()
{
    signal(SIGINT, interruption);

    int id_gniazda;
    int id_gniazda_k;
    int klient_dlugosc;
    int nazwa_gniazda;
    char bufor[MAX];

    struct sockaddr_in adres_serwera;
    // struct sockaddr_un adres_klienta;
    int addrlen = sizeof(adres_serwera);

    id_gniazda = socket(AF_INET, SOCK_STREAM, 0);

    if (id_gniazda == -1)
        blad("Nie moglem utworzyc gniazda.\n");
    else
        printf("Gniazdo zostalo utworzone %d.\n", id_gniazda);

    adres_serwera.sin_family = AF_INET;
    adres_serwera.sin_addr.s_addr = INADDR_ANY;
    adres_serwera.sin_port = htons(21370);

    if (bind(id_gniazda, (struct sockaddr *)&adres_serwera, sizeof(adres_serwera)) == -1)
        blad("Nie moge nadac nazwy gniazdu.\n");
    else
        printf("Gniazdo otrzymalo swoja nazwe.\n");

    if (listen(id_gniazda, 5) == -1)
        blad("Nie moge utworzy kolejki.\n");
    else
        printf("Kolejka zostala utworzona.\n");

    while (1)
    {
        printf("Oczekuje na klienta...\n");
        id_gniazda_k = accept(id_gniazda, (struct sockaddr *)&adres_serwera, (socklen_t *)&addrlen);
        read(id_gniazda_k, &bufor, MAX);
        printf("Otrzymalem znak od klienta : %s\n", bufor);
        for (int i = 0; bufor[i] != '\0'; ++i)
        {
            bufor[i] = toupper(bufor[i]);
        }
        write(id_gniazda_k, &bufor, MAX);
        close(id_gniazda_k);
    }

    system("rm gniazdo_serwera");
}