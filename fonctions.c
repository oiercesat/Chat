#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>

#define TAILLE 1024
#define TAILLE_MESSAGE 140

void *creation_memoire(int socks[10], int newsock, int nbSock);

char* ajoutPseudo(char* pseudo, char* message){
    char* messageAvecPseudo = (char*)malloc(TAILLE_MESSAGE * sizeof(char));
    strcpy(messageAvecPseudo, pseudo);
    strcat(messageAvecPseudo, " : ");
    strcat(messageAvecPseudo, message);
    return messageAvecPseudo;
}

void *creation_memoire(int socks[10], int newsock, int nbSock)
{

    key_t key;
    int shmid;
    char *shmaddr;
    char *cle = (char *)malloc(TAILLE_MESSAGE * sizeof(char));
    char *ptrCle = &cle[0];
    char *extention = ".c";
    FILE *fichier = NULL;
    char *messages_precedants = (char *)malloc(TAILLE * sizeof(char));
    char *message_recu = (char *)malloc(TAILLE * sizeof(char));

    char* pseudo = (char*)malloc(TAILLE_MESSAGE * sizeof(char));

    read(newsock, pseudo, TAILLE_MESSAGE);
    strrchr(pseudo, '\n')[0] = '\0';

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
        messages_precedants = strdup(shmaddr);
        printf("Messages précédants :\n");
        printf("%s", shmaddr);
        write(newsock, shmaddr, TAILLE);
        printf("Vous pouvez maintenant écrire dans le channel %s\n", cle);
        while (1)
        {
            char *message_recu = (char *)malloc(TAILLE * sizeof(char));
            read(newsock, message_recu, TAILLE);

            if (strcmp(message_recu, "exit\n") == 0)
            {
                printf("Vous êtes déconnecté du channel %s\n", cle);
                free(message_recu);
                break;
            }
            else
            {

                // Ajout du pseudo au message
                message_recu = ajoutPseudo(pseudo, message_recu);
                // Réinitialisation et copie du contenu de shmaddr dans messages_precedants
                free(messages_precedants);
                messages_precedants = strdup(shmaddr);

                // Augmentation de la taille de messages_precedants
                messages_precedants = realloc(messages_precedants, strlen(messages_precedants) + strlen(message_recu) + 1);
                strcat(messages_precedants, message_recu);

                // Mise à jour du segment de mémoire partagée
                strcpy(shmaddr, messages_precedants);

                printf("Messages précédants :\n");
                printf("%s", shmaddr);

                for (int i = 0; i < nbSock; i++)
                {
                    if (socks[i] != newsock)
                    {
                        write(socks[i], message_recu, TAILLE);
                    }
                }
                free(message_recu);
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

