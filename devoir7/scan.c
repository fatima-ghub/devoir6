#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define port_max 3000

void scanner(const char *adresse, int port) 
{
    int socketScan = socket(AF_INET, SOCK_STREAM, 0);

    if (socketScan == -1)
    {
        perror("Erreur de création de la socket");
        return;
    }

    struct sockaddr_in addrServer;
    memset(&addrServer, 0, sizeof(struct sockaddr_in));

    addrServer.sin_family = AF_INET;
    addrServer.sin_port = htons(port); 
    addrServer.sin_addr.s_addr = inet_addr(adresse); 

    int resultat = connect(socketScan, (struct sockaddr *)&addrServer, sizeof(struct sockaddr_in));

    if (resultat == 0) 
    {
        printf("Port %d ouvert\n", port); 
    }

    close(socketScan);
}

int main(int argc, char *argv[]) 
{
    if (argc < 3)
    {
        printf("Erreur : Nombre d'arguments insuffisant.\n");
        printf("Usage : %s adresse_IP PORT\n", argv[0]);
        return 0;
    }

    const char *adresse = argv[1]; 
    int port_fourni = atoi(argv[2]); 

    if (port_fourni < 0 || port_fourni > port_max)
    {
        printf("Erreur : Le port doit être compris entre 0 et %d.\n", port_max);
        return 0;
    }

    for (int port = port_fourni; port <= port_max; port++) 
    {
        scanner(adresse, port); 
    }

    return 0;
}
