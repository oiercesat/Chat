#include <netinet/in.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <ctype.h>
#include <stdbool.h>

#include "fonctions.c"

void arret(int sig);
void *creation_memoire_conteneur(void *arg);

/* -------------------------------------------------------------------------- */

struct Serveur{
    int sock;
    int newsock;
    int nbSock;
    int socks[10];
};

int sock=0;

int main(){

    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(atoi("8080"));
    sin.sin_addr.s_addr = inet_addr("127.01.01.01");

    socklen_t sin_len = sizeof(sin);

    sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock == -1)
    {
        printf("Erreur lors de la création de la socket\n");
        return -1;
    }

    if (bind(sock, (struct sockaddr *)&sin, sizeof(sin)) == -1)
    {
        perror("Erreur lors de la liaison (bind) de la socket");
        return -1;
    }

    if(listen(sock, 0) == -1){
        perror("Erreur lors de l'écoute");
        return -1;
    }

    struct Serveur serveur;
    serveur.sock = sock;
    serveur.newsock = 0;
    serveur.nbSock = 0;

    while (1)
    {
        signal(SIGINT, &arret);

        pthread_t thread;

        serveur.newsock = accept(sock, (struct sockaddr *)&sin, &sin_len);
        if (serveur.newsock == -1)
        {
            perror("Erreur lors de l'acceptation");
            return -1;
        }
        else
        {
            printf("Acceptation du client\n");
            pthread_create(&thread, NULL, creation_memoire_conteneur, (void*)&serveur);
            serveur.socks[serveur.nbSock] = serveur.newsock;
            serveur.nbSock++;
        }
    }
    
    close(sock);
}

/* -------------------------------------------------------------------------- */
/*    FONCTIONS SERVEUR NECESSITANT LES VARIABLES GLOBALES sock et newsock    */
/* -------------------------------------------------------------------------- */

void *creation_memoire_conteneur(void * arg){
    struct Serveur *serveur = (struct Serveur *)arg;
    creation_memoire(serveur->socks, serveur->newsock, serveur->nbSock);
    return NULL;
}

void arret(int sig){
    printf("\nFermeture de la socket\n");
    close(sock);
    exit(0);
}
