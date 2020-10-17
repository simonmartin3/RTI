/* TCPCLI01.C
* client *
- Claude Vilvens -
*/
#include <stdio.h>
#include <stdlib.h> /* pour exit */
#include <string.h> /* pour memcpy */
#include <sys/types.h>
#include <sys/socket.h> /* pour les types de socket */
#include <netdb.h> /* pour la structure hostent */
#include <errno.h>
#include <netinet/in.h> /* conversions adresse reseau-format dot et local/ reseau */
#include <netinet/tcp.h> /* pour la conversion adresse reseau-format dot */
#include <arpa/inet.h> /* pour la conversion adresse reseau-format dot */
#include <signal.h>
#include <pthread.h>
#include "socketLib.h"
#include "utilLib.h"
#include "CMMP.h"

#define MAXSTRING 100 /* Longeur des messages */
#define EOC "END_OF_CONNEXION"
#define OK "true"
#define FAIL "false"
#define FILELOG "login.csv"
#define FILEPARC "FICH_PARC"

void HandlerSIGUSR1(int);
void HandlerSIGUSR2(int);

int PORT;

int main()
{
    int hSocket; /* Handle de la socket */
    //struct hostent * infosHost; /*Infos sur le host : pour gethostbyname */
    //struct in_addr adresseIP; /* Adresse Internet au format reseau */
    struct sockaddr_in adresseSocket; /* Structure de type sockaddr - ici, cas de TCP */
    unsigned int tailleSockaddr_in;
    int ret, option, end = 0, endList = 0; /* valeur de retour */
    char msgClient[MAXSTRING], msgServeur[MAXSTRING];
	Message msgSend;
    char tmp[20];

/* Ouverture fichier config */
    memcpy(tmp, searchConfig("PORT_CLIENT"), sizeof(tmp));
    PORT = atoi(tmp);
    
/* 1. Création de la socket */
    hSocket = SocketInit(&adresseSocket, "ubuntu", PORT);

/* 4. Tentative de connexion */
    tailleSockaddr_in = sizeof(struct sockaddr_in);

    if ((ret = connect(hSocket, (struct sockaddr *)&adresseSocket, tailleSockaddr_in) ) == -1)
    {
        printf("Erreur sur connect de la socket %d\n", errno);
        close(hSocket);
        exit(1);
    }
    else 
        printf("Connect socket OK\n");

/* 5.Envoi d'un message client */
    do
    {
    	msgSend = login();

    	memcpy(msgClient, &msgSend, sizeof(struct Message));

        if (send(hSocket, msgClient, MAXSTRING, 0) == -1) /* pas message urgent */
        {
            printf("Erreur sur le send de la socket %d\n", errno);
            close(hSocket); /* Fermeture de la socket */
            exit(1);
        }
        else 
            printf("Send socket OK\n");

    /* 6. Reception de l'ACK du serveur au client */
        if (recv(hSocket, msgServeur, MAXSTRING, 0) == -1)
        {
            printf("Erreur sur le recv de la socket %d\n", errno);
            close(hSocket); /* Fermeture de la socket */
            exit(1);
        }
        else 
            printf("Recv socket OK\n");

        printf("Message recu en ACK = %s\n", msgServeur);
    }while(strcmp(msgServeur, OK) != 0);

/* 7.Choix d'une action du client */
    do
    {
        do
        {
            system("clear");
            printf("1 - Signalement qu'un camion arrive.\n");
            printf("2 - Signalement vehicule disponible.\n");
            printf("3 - Signalement container charge.\n");
            printf("4 - LOGOUT.\n");
            printf("5 - Afficher containers\n");
            printf("6 - Afficher vehicule\n");
            printf("Veuillez selectionner une option :");
            scanf("%d", &option);
            fflush(stdin);
            option = (int)option;
        }while(option < 1 || option > 7);

        switch(option)
        {
            case 1 :
                msgSend = inputTruck();
                break;

            case 2 :
                msgSend = outputReady();
                break;

            case 3 :
                msgSend = outputOne(); 
                break;

            case 4 :
                msgSend = logout();
                break;

            case 5 :
                msgSend.typeReq = 7;
                strcpy(msgSend.msg, "");
                break;
            case 6 :
                msgSend.typeReq = 8;
                strcpy(msgSend.msg, "");
                break;
        }

        memcpy(msgClient, &msgSend, sizeof(struct Message));
        
        if (send(hSocket, msgClient, MAXSTRING, 0) == -1) /* pas message urgent */
        {
            printf("Erreur sur le send de la socket %d\n", errno);
            close(hSocket); /* Fermeture de la socket */
            exit(1);
        }
        else 
            printf("Send socket OK\n");

        if(option == 1)
        {
            if (recv(hSocket, msgServeur, MAXSTRING, 0) == -1)
            {
                printf("Erreur sur le recv de la socket %d\n", errno);
                close(hSocket); /* Fermeture de la socket */
                exit(1);
            }
            else 
                printf("Recv socket OK\n");

            printf("%s\n", msgServeur);

            if(strcmp(msgServeur, FAIL) != 0)
            {                
                msgSend = inputDone(msgServeur);
                memcpy(msgClient, &msgSend, sizeof(struct Message));

                if (send(hSocket, msgClient, MAXSTRING, 0) == -1) /* pas message urgent */
                {
                    printf("Erreur sur le send de la socket %d\n", errno);
                    close(hSocket); /* Fermeture de la socket */
                    exit(1);
                }
                else 
                    printf("Send socket OK\n");
            }

            if (recv(hSocket, msgServeur, MAXSTRING, 0) == -1)
            {
                printf("Erreur sur le recv de la socket %d\n", errno);
                close(hSocket); /* Fermeture de la socket */
                exit(1);
            }
            else 
                printf("Recv socket OK\n");
        }

        if(option == 2)
        {
            system("clear");
            fflush(stdin);
            do
            {
                if (recv(hSocket, msgServeur, MAXSTRING, 0) == -1)
                {
                    printf("Erreur sur le recv de la socket %d\n", errno);
                    close(hSocket); /* Fermeture de la socket */
                    exit(1);
                }

                if(strcmp(msgServeur, OK) == 0)
                    endList = 1;
                else
                    printf("%s\n", msgServeur);

            }while(endList != 1);

            pressEnter();

            if (recv(hSocket, msgServeur, MAXSTRING, 0) == -1)
            {
                printf("Erreur sur le recv de la socket %d\n", errno);
                close(hSocket); /* Fermeture de la socket */
                exit(1);
            }
            else 
                printf("Recv socket OK\n");

            printf("Message recu en ACK = %s\n", msgServeur);
        }

        if(option == 3)
        {
            if (recv(hSocket, msgServeur, MAXSTRING, 0) == -1)
            {
                printf("Erreur sur le recv de la socket %d\n", errno);
                close(hSocket); /* Fermeture de la socket */
                exit(1);
            }
            else 
                printf("Recv socket OK\n");

            printf("%s\n", msgServeur);

            if(strcmp(msgServeur, OK) != 0 && strcmp(msgServeur, FAIL) != 0)
            {       
                msgSend = outputDone(msgServeur);
                memcpy(msgClient, &msgSend, sizeof(struct Message));

                if (send(hSocket, msgClient, MAXSTRING, 0) == -1) /* pas message urgent */
                {
                    printf("Erreur sur le send de la socket %d\n", errno);
                    close(hSocket); /* Fermeture de la socket */
                    exit(1);
                }
                else 
                    printf("Send socket OK\n");

                printf("Aucun vehicule dispo\n");
                pressEnter();
            }
            else
            {
                if(strcmp(msgServeur, OK) == 0)
                {
                    printf("Le container a bien ete charge\n");
                }
                else
                {
                    printf("Erreur lors du chargement\n");
                }
            }
        }

        if(option == 4)
        {
            if (recv(hSocket, msgServeur, MAXSTRING, 0) == -1)
            {
                printf("Erreur sur le recv de la socket %d\n", errno);
                close(hSocket); /* Fermeture de la socket */
                exit(1);
            }
            else 
                printf("Recv socket OK\n");

            printf("Message recu en ACK = %s\n", msgServeur);

            if (strcmp(msgServeur, EOC)==0)
            {
                if (send(hSocket, EOC, MAXSTRING, 0) == -1) /* pas message urgent */
                {
                    printf("Erreur sur le send de la socket %d\n", errno);
                    close(hSocket); /* Fermeture de la socket */
                    exit(1);
                }
                else 
                    printf("Send socket OK\n");

                end=1;
            }
        }

        if(option == 5 || option == 6)
        {
            if (recv(hSocket, msgServeur, MAXSTRING, 0) == -1)
            {
                printf("Erreur sur le recv de la socket %d\n", errno);
                close(hSocket); /* Fermeture de la socket */
                exit(1);
            }
            else 
                printf("Recv socket OK\n");

            printf("Message recu en ACK = %s\n", msgServeur);
        }

    }while(end != 1);

/* 9. Fermeture de la socket */
    close(hSocket); /* Fermeture de la socket */
    printf("Socket client fermee\n");
    
    return 0;
}

void HandlerSIGUSR1(int sig)
{
    printf("Interrupt\n");
}

void HandlerSIGUSR2(int sig)
{
    printf("Stop\n");
    exit(1);
}

