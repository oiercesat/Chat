#include <netinet/in.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define TAILLE 140


int main( int argc, char *argv[]){

    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(atoi("8081"));
    sin.sin_addr.s_addr = inet_addr("127.01.01.01");

    int sock;

    char *messageEnvoyer = (char *)malloc(1024 * sizeof(char)); 

    sock = socket(AF_INET, SOCK_STREAM, 0);

    if (connect(sock, (struct sockaddr *)&sin, sizeof(sin)) == -1)
    {
        printf("Vous n'êtes pas connecté au serveur \n");
        close(sock);
        return -1;
    }
    else
    {
        printf("Vous êtes connecté au serveur \n");

        char* cle =(char*)malloc(1024*sizeof(char));
        char* ptrCle = &cle[0];

        printf("Veuillez saisir un serveur\n");
        fgets(cle, TAILLE, stdin);
        write(sock, ptrCle, TAILLE);

        printf("Veuillez saisir un message\n");
        while (strcmp(messageEnvoyer, "exit\n") != 0)
        {
            fgets(messageEnvoyer, TAILLE, stdin);
            write(sock, messageEnvoyer, TAILLE);
        }        
    }

    close(sock);
    
}