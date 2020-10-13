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
#include "socketLib.h"
#include "CMMP.h"

#define MAXSTRING 100 /* Longeur des messages */
#define EOC "END_OF_CONNEXION"
#define FILELOG "login.csv"
#define FILEPARC "FICH_PARC"

void showContainer();

int PORT;

typedef struct Container Container;
struct Container
{
    char idContainer[20];
    char coordonnees[10];
    int etat;
    char dateReservation[20];
    char dateArrivee[20];
    char destination[50];
    int poids;
    char typeRetour[10];

};

int main()
{
    int hSocket; /* Handle de la socket */
    struct hostent * infosHost; /*Infos sur le host : pour gethostbyname */
    struct in_addr adresseIP; /* Adresse Internet au format reseau */
    struct sockaddr_in adresseSocket; /* Structure de type sockaddr - ici, cas de TCP */
    unsigned int tailleSockaddr_in;
    int ret, option, end = 0; /* valeur de retour */
    char msgClient[MAXSTRING], msgServeur[MAXSTRING];
	char * msgTmp = (char *)malloc(MAXSTRING);
    char **param = NULL;
    int port;
    char tmp[20];

/* Ouverture fichier config */
    memcpy(tmp, searchConfig("PORT_SERVEUR"), sizeof(tmp));
    port = atoi(tmp);
    

    
/* 1. Création de la socket */
    hSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (hSocket == -1)
    {
        printf("Erreur de creation de la socket %d\n", errno);
        exit(1);
    }
    else 
    printf("Creation de la socket OK\n");

/* 2. Acquisition des informations sur l'ordinateur distant */
    if ( (infosHost = gethostbyname("solaris11DM2017"))==0)
    {
        printf("Erreur d'acquisition d'infos sur le host distant %d\n", errno);
        exit(1);
    }
    else 
        printf("Acquisition infos host distant OK\n");

    memcpy(&adresseIP, infosHost->h_addr, infosHost->h_length);
    printf("Adresse IP = %s\n",inet_ntoa(adresseIP));

/* 3. Préparation de la structure sockaddr_in */
    memset(&adresseSocket, 0, sizeof(struct sockaddr_in));
    adresseSocket.sin_family = AF_INET; /* Domaine */
    adresseSocket.sin_port = htons(port);
    /* conversion numéro de port au format réseau */
    memcpy(&adresseSocket.sin_addr, infosHost->h_addr,infosHost->h_length);

/* 4. Tentative de connexion */
    tailleSockaddr_in = sizeof(struct sockaddr_in);

    if (( ret = connect(hSocket, (struct sockaddr *)&adresseSocket, tailleSockaddr_in) ) == -1)
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
    	msgTmp = login();

    	strcpy(msgClient, msgTmp);

        if (send(hSocket, msgClient, MAXSTRING, 0) == -1) /* pas message urgent */
        {
            printf("Erreur sur le send de la socket %d\n", errno);
            close(hSocket); /* Fermeture de la socket */
            exit(1);
        }
        else 
            printf("Send socket OK\n");

        printf("Message envoye = %s\n", msgClient);

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
    }while(strcmp(msgServeur, "true") != 0);

/* 7.Choix d'une action du client */
    do
    {
        do
        {
            //system("clear");
            printf("1 - Signalement qu'un camion arrive.\n");
            printf("2 - Signalement vehicule disponible.\n");
            printf("3 - Signalement container charge.\n");
            printf("4 - Signalement maximum container.\n");
            printf("5 - LOGOUT.\n");
            printf("6 - Afficher containers\n");
            printf("Veuillez selectionner une option :");
            scanf("%d", &option);
            fflush(stdin);
            option = (int)option;
        }while(option < 1 || option > 7);

        switch(option)
        {
            case 1 :
                msgTmp = inputTruck();
                break;

            case 2 :
                msgTmp = outputReady();
                break;

            case 5 :
                msgTmp = logout();
                break;

            case 6 :
                msgTmp = "8";
                break;
        }

        strcpy(msgClient, msgTmp);
        
        if (send(hSocket, msgClient, MAXSTRING, 0) == -1) /* pas message urgent */
        {
            printf("Erreur sur le send de la socket %d\n", errno);
            close(hSocket); /* Fermeture de la socket */
            exit(1);
        }
        else 
            printf("Send socket OK\n");

        printf("Message envoye = %s\n", msgClient);

    /* 8. Reception de l'ACK du serveur au client */
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

        if(option == 2)
        {
            Container *tmp;
            memcpy(tmp, msgServeur, sizeof(msgServeur));
            printf("%s\n", tmp[0]->idContainer);
        }

        param = tokenizer(msgServeur, "#;");

        if (strcmp(param[0], "1") ==0)
        {
            msgTmp = inputDone();
            strcat(msgTmp, ";");
            strcat(msgTmp, param[2]);

            strcpy(msgClient, msgTmp);

            if (send(hSocket, msgClient, MAXSTRING, 0) == -1) /* pas message urgent */
            {
                printf("Erreur sur le send de la socket %d\n", errno);
                close(hSocket); /* Fermeture de la socket */
                exit(1);
            }
            else 
                printf("Send socket OK\n");

            do
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

            }while(strcmp(msgServeur, "OK") != 0);
        }

    }while(end != 1);

/* 9. Fermeture de la socket */
    close(hSocket); /* Fermeture de la socket */
    printf("Socket client fermee\n");
    
    return 0;
}


