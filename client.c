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
    sin.sin_port = htons(atoi("8080"));
    sin.sin_addr.s_addr = inet_addr("127.01.01.01");

    int sock;

    char *messageEnvoyer = (char *)malloc(1024 * sizeof(char)); 
    char *messages_precedents = (char *)malloc(1024 * sizeof(char));
    char *message_recu = (char *)malloc(1024 * sizeof(char));

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

        printf("Messages précédents : \n");
        read(sock, messages_precedents, 1024);
        printf("%s", messages_precedents);

        printf("Veuillez saisir un message\n");
        while (1) {
            fgets(messageEnvoyer, TAILLE, stdin);
            write(sock, messageEnvoyer, TAILLE);

            if (strcmp(messageEnvoyer, "exit\n") == 0) {
                break;
            }

            read(sock, message_recu, TAILLE);
            printf("Message reçu : \n");
            printf("%s\n", message_recu);
        }
    }

    close(sock);
    free(messageEnvoyer);
    free(messages_precedents);
    free(message_recu);

    return 0;
}