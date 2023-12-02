#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>

#define TAILLE 1024
#define TAILLE_MESSAGE 140

/**
 * Fonction qui ajoute le pseudo au message
 * @param pseudo le pseudo de l'utilisateur
 * @param message le message à envoyer
 * @return le message avec le pseudo
 */
char *ajoutPseudo(char *pseudo, char *message)
{
    char *messageAvecPseudo = (char *)malloc(TAILLE_MESSAGE * sizeof(char));
    strcpy(messageAvecPseudo, pseudo);
    strcat(messageAvecPseudo, " : ");
    strcat(messageAvecPseudo, message);
    return messageAvecPseudo;
}

/**
 * Fonction qui envoie les messages aux autres utilisateurs
 * @param socks les sockets des utilisateurs
 * @param newsock la socket de l'utilisateur
 * @param nbSock le nombre de sockets
 * @param shmaddr le segment de mémoire partagée
 * @param messages_precedants les messages précédants
 * @param message_recu le message reçu
 * @param pseudo le pseudo de l'utilisateur
 * @return void
 * @see ajoutPseudo
 */
void envoiMessages(int socks[10], int newsock, int nbSock, char *shmaddr, char *messages_precedants, char *message_recu, char *pseudo)
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

    // Affichage des messages précédants dans la console du serveur
    printf("Messages précédants :\n");
    printf("%s", shmaddr);

    // Envoi du message aux autres utilisateurs
    for (int i = 0; i < nbSock; i++)
    {
        // Si la socket est différente de celle de l'utilisateur
        if (socks[i] != newsock)
        {
            write(socks[i], message_recu, TAILLE);
        }
    }
    free(message_recu);
}

/**
 * Fonction qui crée le segment de mémoire partagée
 * @param socks les sockets des utilisateurs
 * @param newsock la socket de l'utilisateur
 * @param nbSock le nombre de sockets
 * @return void
 * @see envoiMessages
 * @see ajoutPseudo
 *
 */
void *creation_memoire(int socks[10], int newsock, int nbSock)
{
    // Variables de la mémoire partagée
    key_t key;
    int shmid;
    char *shmaddr;

    // Variables de la clé
    char *cle = (char *)malloc(TAILLE_MESSAGE * sizeof(char));
    char *ptrCle = &cle[0];

    // Variables du fichier
    char *extention = ".c";
    FILE *fichier = NULL;

    // Variables des messages
    char *messages_precedants = (char *)malloc(TAILLE * sizeof(char));
    char *message_recu = (char *)malloc(TAILLE * sizeof(char));
    char *pseudo = (char *)malloc(TAILLE_MESSAGE * sizeof(char));

    // Récupération du pseudo
    read(newsock, pseudo, TAILLE_MESSAGE);
    strrchr(pseudo, '\n')[0] = '\0';

    // Récupération de la clé
    read(newsock, ptrCle, TAILLE_MESSAGE);
    strrchr(cle, '\n')[0] = '\0';
    strcat(cle, extention);

    // Vérification de l'existence du fichier qui correspond à la clé
    if ((fichier = fopen(cle, "r")))
    {
        printf("Le channel %s existe déjà\n", cle);
        fclose(fichier);

        // Récupération de la clé
        key = ftok(cle, 'A');
        if (key == -1)
        {
            printf("Erreur lors de la création de la clé\n");
            exit(1);
        }

        // Récupération du segment de mémoire partagée
        shmid = shmget(key, TAILLE, 0);

        if (shmid == -1)
        {
            printf("Erreur lors de la création du segment de mémoire partagée\n");
            exit(1);
        }

        // Récupération de l'adresse du segment de mémoire partagée
        shmaddr = shmat(shmid, NULL, 0);
        if (shmaddr == (char *)-1)
        {
            printf("Erreur lors de l'attachement du segment de mémoire partagée\n");
            exit(1);
        }

        printf("Vous êtes connecté au channel %s\n", cle);
        messages_precedants = strdup(shmaddr);
        // Affichage des messages précédants dans la console du serveur
        printf("Messages précédants :\n");
        printf("%s", shmaddr);
        // Envoi des messages précédants à l'utilisateur
        write(newsock, shmaddr, TAILLE);
        printf("Vous pouvez maintenant écrire dans le channel %s\n", cle);
        while (1)
        {
            // Récupération du message de l'utilisateur
            char *message_recu = (char *)malloc(TAILLE * sizeof(char));
            read(newsock, message_recu, TAILLE);

            // Si l'utilisateur veut quitter le channel
            if (strcmp(message_recu, "exit\n") == 0)
            {
                printf("Vous êtes déconnecté du channel %s\n", cle);
                free(message_recu);
                break;
            }
            else
            {
                envoiMessages(socks, newsock, nbSock, shmaddr, messages_precedants, message_recu, pseudo);
            }
        }
    }
    else
    {
        // Création du fichier associé au channel si il n'existe pas
        printf("Le channel %s n'existe pas\n", cle);
        fichier = fopen(cle, "w");
        fclose(fichier);

        // Création de la clé
        key = ftok(cle, 'A');
        if (key == -1)
        {
            printf("Erreur lors de la création de la clé\n");
            exit(1);
        }

        // Création du segment de mémoire partagée
        shmid = shmget(key, TAILLE, IPC_CREAT | 0666 | IPC_EXCL);
        if (shmid == -1)
        {
            perror("shmget");
            exit(1);
        }

        // Récupération de l'adresse du segment de mémoire partagée
        shmaddr = shmat(shmid, NULL, 0);
        if (shmaddr == (char *)-1)
        {
            printf("Erreur lors de l'attachement du segment de mémoire partagée\n");
            exit(1);
        }

        printf("Vous venez de creer le channel %s\n", cle);
        while (1)
        {
            // Récupération du message de l'utilisateur
            char *message_recu = (char *)malloc(TAILLE * sizeof(char));
            read(newsock, message_recu, TAILLE);
            if (strcmp(message_recu, "exit\n") == 0)
            {
                printf("Vous êtes déconnecté du channel %s\n", cle);
                break;
            }
            else
            {
                envoiMessages(socks, newsock, nbSock, shmaddr, messages_precedants, message_recu, pseudo);
            }
        }
    }

    // Détachement du segment de mémoire partagée
    shmdt(shmaddr);
    // Suppression du segment de mémoire partagée
    shmctl(shmid, IPC_RMID, NULL);
    // Suppression du fichier associé au channel
    remove(cle);
    // Fermeture de la socket
    close(newsock);
    return 0;
}
