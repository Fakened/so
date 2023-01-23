#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#define MAX 15

static void blad(char opis[])
{
    printf("%s/n", opis);
    exit(EXIT_FAILURE);
}

int main()
{
    int id_gniazda;
    int serwer_dlugosc;
    int nazwa_gniazda;
    char znak;

    // nazywamy gniazdo serwera
    struct sockaddr_in adres_serwera;

    id_gniazda = socket(AF_INET, SOCK_STREAM, 0);
    if (id_gniazda == -1)
        blad("Nie moglem utworzyc gniazda.");
    else
        printf("Gniazdo zostalo utworzone %d.\n", id_gniazda);

    adres_serwera.sin_family = AF_INET;
    adres_serwera.sin_port = htons(21370);
    inet_pton(AF_INET, "149.156.136.154", &adres_serwera.sin_addr);

    if (connect(id_gniazda, (struct sockaddr *)&adres_serwera, sizeof(adres_serwera)) == -1)
        blad("Nie moge nadac nazwy gniazdu.");
    else
        printf("Gniazdo otrzymalo swoja nazwe.\n");

    int iterator = 0;
    char bufor[MAX];
    while ((znak = fgetc(stdin)) != '\n')
    {
        if (iterator < (MAX - 1))
        {
            bufor[iterator] = znak;
        }
        ++iterator;
    }
    if (iterator >= (MAX - 1))
    {
        bufor[MAX - 1] = '\0';
    }
    else
    {
        bufor[iterator] = '\0';
    }

    write(id_gniazda, &bufor, MAX);
    read(id_gniazda, &bufor, MAX);
    printf("Otrzymalem znak od serwera : %s\n", bufor);

    close(id_gniazda);
    exit(0);
}