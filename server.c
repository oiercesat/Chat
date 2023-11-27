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

/* -------------------------------------------------------------------------- */

int sock=0;
int newsock=0;

int main(){

    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(atoi("8081"));
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

    while (1)
    {
        signal(SIGINT, &arret);

        pthread_t thread;

        newsock = accept(sock, (struct sockaddr *)&sin, &sin_len);
        if (newsock == -1)
        {
            perror("Erreur lors de l'acceptation");
            return -1;
        }
        else
        {
            printf("Acceptation du client\n");
            pthread_create(&thread, NULL, creation_memoire, (void*)&newsock);
        }
    }
    
    close(sock);
}

/* -------------------------------------------------------------------------- */
/*    FONCTIONS SERVEUR NECESSITANT LES VARIABLES GLOBALES sock et newsock    */
/* -------------------------------------------------------------------------- */

void arret(int sig){
    printf("\nFermeture de la socket\n");
    close(sock);
    exit(0);
}
