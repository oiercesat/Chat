#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>

#define TAILLE 1024
#define TAILLE_MESSAGE 140

void *creation_memoire(void *sock);

void *creation_memoire(void *sock)
{

    int newsock = *(int *)sock;
    key_t key;
    int shmid;
    char *shmaddr;
    char *cle = (char *)malloc(TAILLE_MESSAGE * sizeof(char));
    char *ptrCle = &cle[0];
    char *extention = ".c";
    FILE *fichier = NULL;
    char *messages_precedants = (char *)malloc(TAILLE_MESSAGE * sizeof(char));
    char *message_recu = (char *)malloc(TAILLE_MESSAGE * sizeof(char));

    read(newsock, ptrCle, TAILLE_MESSAGE);
    strrchr(cle, '\n')[0] = '\0';
    strcat(cle, extention);

    if ((fichier = fopen(cle, "r")))
    {
        printf("Le channel %s existe déjà\n", cle);
        fclose(fichier);
        key = ftok(cle, 'A');
        if (key == -1)
        {
            printf("Erreur lors de la création de la clé\n");
            exit(1);
        }

        shmid = shmget(key, TAILLE, 0);

        if (shmid == -1)
        {
            printf("Erreur lors de la création du segment de mémoire partagée\n");
            exit(1);
        }
        shmaddr = shmat(shmid, NULL, 0);
        if (shmaddr == (char *)-1)
        {
            printf("Erreur lors de l'attachement du segment de mémoire partagée\n");
            exit(1);
        }

        printf("Vous êtes connecté au channel %s\n", cle);
        strcpy(messages_precedants, shmaddr);
        printf("Messages précédants :\n");
        printf("%s", shmaddr);
        printf("Vous pouvez maintenant écrire dans le channel %s\n", cle);
        while (1)
        {
            read(newsock, message_recu, TAILLE_MESSAGE);
            if (strcmp(message_recu, "exit\n") == 0)
            {
                printf("Vous êtes déconnecté du channel %s\n", cle);
                break;
            }
            else
            {
                printf("%s", message_recu);
                strcat(messages_precedants, message_recu);
                strcpy(shmaddr, messages_precedants);
                printf("Messages précédants :\n");
                printf("%s", shmaddr);
            }
        }
    }
    else
    {
        printf("Le channel %s n'existe pas\n", cle);
        fichier = fopen(cle, "w");
        fclose(fichier);
        key = ftok(cle, 'A');
        if (key == -1)
        {
            printf("Erreur lors de la création de la clé\n");
            exit(1);
        }

        shmid = shmget(key, TAILLE, IPC_CREAT | 0666 | IPC_EXCL);
        if (shmid == -1)
        {
            perror("shmget");
            exit(1);
        }

        shmaddr = shmat(shmid, NULL, 0);
        if (shmaddr == (char *)-1)
        {
            printf("Erreur lors de l'attachement du segment de mémoire partagée\n");
            exit(1);
        }

        printf("Vous venez de creer le channel %s\n", cle);
        while (1)
        {
            read(newsock, message_recu, TAILLE_MESSAGE);
            if (strcmp(message_recu, "exit\n") == 0)
            {
                printf("Vous êtes déconnecté du channel %s\n", cle);
                break;
            }
            else
            {
                printf("%s", message_recu);
            }
        }
        
    }

    close(newsock);
    return 0;
}
