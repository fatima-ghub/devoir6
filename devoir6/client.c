#include <stdio.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "common.h"

int main(int argc, char *argv[]) 
{
    if (argc < 3) 
    {
        printf("Erreur : nombre d'arguments insuffisant \n");
        printf("Usage : %s adresse_IP PORT \n", argv[0]);
        return 0;
    }

    int socketClient = socket(AF_INET, SOCK_STREAM, 0);
    if (socketClient == -1) {
        perror("Erreur de création de la socket");
        return 0;
    }

    printf("[#Client#] Socket créée avec succès\n");

    struct sockaddr_in addrServer;
    memset(&addrServer, 0, sizeof(struct sockaddr_in));
    addrServer.sin_family = AF_INET;
    addrServer.sin_port = htons(atoi(argv[2]));
    addrServer.sin_addr.s_addr = inet_addr(argv[1]);

    if (connect(socketClient, (struct sockaddr *)&addrServer, sizeof(addrServer)) == -1) 
    {
        perror("Erreur de connexion");
        return 0;
    }

    printf("[#Client#] Connexion au serveur réussie\n");

    char buffReceive[buff];
    while (1) {
        memset(buffReceive, 0, buff);
        if (recv(socketClient, buffReceive, buff, 0) <= 0) 
        {
            perror("Erreur de réception");
            break;
        }

        printf("[#Client#] Compteur actuel reçu du serveur: %s\n", buffReceive);

        int compteur = atoi(buffReceive);
        compteur++;  
        snprintf(buffReceive, buff, "%d", compteur);
        if (send(socketClient, buffReceive, strlen(buffReceive), 0) <= 0) 
        {
            perror("Erreur d'envoi");
            break;
        }

        printf("[#Client#] Compteur envoyé au serveur: %d\n", compteur);

        sleep(1);  
    }

    close(socketClient);
    return 0;
}
