#include <netinet/in.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define TAILLE 140

int sock;
char *messages_precedents;

void *receiveMessages(void *arg)
{
    char *message_recu = (char *)malloc(TAILLE * sizeof(char));
    while (1)
    {
        read(sock, message_recu, TAILLE);
        printf("%s", message_recu);
    }
    free(message_recu);
}

int main(int argc, char *argv[])
{
    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(atoi("8081"));
    sin.sin_addr.s_addr = inet_addr("127.01.01.01");

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

        char* pseudo = (char*)malloc(TAILLE * sizeof(char));
        printf("Veuillez saisir un pseudo\n");
        fgets(pseudo, TAILLE, stdin);
        write(sock, pseudo, TAILLE);

        char cle[1024];
        printf("Veuillez saisir un serveur\n");
        fgets(cle, TAILLE, stdin);
        write(sock, cle, TAILLE);

        messages_precedents = (char *)malloc(1024 * sizeof(char));

        pthread_t receiveThread;
        pthread_create(&receiveThread, NULL, receiveMessages, NULL);

        printf("Messages précédents : \n");
        while (1)
        {
            char messageEnvoyer[TAILLE];
            fgets(messageEnvoyer, TAILLE, stdin);
            write(sock, messageEnvoyer, TAILLE);

            if (strcmp(messageEnvoyer, "exit\n") == 0)
                break;
        }

        pthread_cancel(receiveThread); // Arrêt du thread de réception
        free(messages_precedents);
    }

    close(sock);

    return 0;
}
