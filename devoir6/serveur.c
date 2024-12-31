#include <stdio.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include "common.h"

int compteur = 0;
sem_t sem_compteur;

void *gesClient(void *arg) 
{
    int socketClient = *(int *)arg;
    free(arg);
    char buffReceive[buff];
    char buffSend[buff];

    while (1) 
    {  
        sem_wait(&sem_compteur);

        snprintf(buffSend, buff, "%d", compteur);
        if (send(socketClient, buffSend, strlen(buffSend), 0) <= 0) 
        {
            perror("Erreur d'envoi");
            break;
        }
        printf("[#Serveur#] Compteur envoyé au client: %d\n", compteur);

        memset(buffReceive, 0, buff);
        if (recv(socketClient, buffReceive, buff, 0) <= 0) 
        {
            perror("Erreur de réception");
            break;
        }

        if (strlen(buffReceive) == 0) break;  

        compteur = atoi(buffReceive);
        compteur++;  
        printf("[#Serveur#] Nouveau compteur: %d\n", compteur);

        sleep(1);  

        sem_post(&sem_compteur);
    }

    close(socketClient);
    return NULL;
}

int main(int argc, char *argv[]) 
{
    if (argc < 3) {
        printf("Erreur : nombre d'arguments insuffisant \n");
        printf("Usage : %s adresse_IP PORT \n", argv[0]);
        return 0;
    }

    sem_init(&sem_compteur, 0, 1); 

    int socketServer = socket(AF_INET, SOCK_STREAM, 0);
    if (socketServer == -1) {
        perror("Erreur de création de la socket");
        return 0;
    }

    printf("[#Serveur#] Socket créée avec succès\n");

    struct sockaddr_in addrServer;
    memset(&addrServer, 0, sizeof(struct sockaddr_in));
    addrServer.sin_family = AF_INET;
    addrServer.sin_port = htons(atoi(argv[2]));
    addrServer.sin_addr.s_addr = inet_addr("0.0.0.0");

    if (bind(socketServer, (struct sockaddr *)&addrServer, sizeof(sockaddr_in)) == -1) 
    {
        perror("Erreur de lancement de BIND");
        return 0;
    }

    printf("[#Serveur#] Execution de BIND avec succès\n");

    if (listen(socketServer, 5) == -1) 
    {
        perror("Erreur de lancement de listen");
        return 0;
    }

    printf("[#Serveur#] Execution de listen avec succes...\n");

    while (1) {
        struct sockaddr_in addrClient;
        int taille = sizeof(struct sockaddr_in);
        int socketClient = accept(socketServer, (struct sockaddr *)&addrClient, &taille);
        if (socketClient == -1) {
            perror("Erreur de ACCEPT");
            return 0;
        }

        pthread_t thread;
        int *thread_socketClient = malloc(sizeof(int));
        *thread_socketClient = socketClient;

        if (pthread_create(&thread, NULL, gesClient, thread_socketClient) != 0) {
            perror("Erreur de création du thread");
        }

        pthread_detach(thread); 
    }

    close(socketServer);
    sem_destroy(&sem_compteur); 
    return 0;
}
