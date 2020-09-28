/* tcpVarCond.c
-- Claude Vilvens --
*/
#include <stdio.h>
#include <stdlib.h> /* pour exit */
#include <string.h> /* pour memcpy */

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h> /* pour les types de socket */
#include <netdb.h> /* pour la structure hostent */
#include <errno.h>
#include <netinet/in.h> /* pour la conversion adresse reseau - format dot ainsi que le conversion format local/format reseau */
#include <netinet/tcp.h> /* pour la conversion adresse reseau - format dot */
#include <arpa/inet.h> /* pour la conversion adresse reseau - format dot */
#include <time.h> /* pour select et timeval */
#include <pthread.h>

#define NB_MAX_CLIENTS 2 /* Nombre maximum de clients connectes */
#define EOC "END_OF_CONNEXION"
#define DOC "DENY_OF_CONNEXION"
#define PORT 50000 /* Port d'ecoute de la socket serveur */
#define MAXSTRING 100 /* Longueur des messages */
#define affThread(num, msg) printf("th_%s> %s\n", num, msg)

pthread_mutex_t mutexIndiceCourant;
pthread_cond_t condIndiceCourant;
int indiceCourant=-1;
pthread_t threadHandle[NB_MAX_CLIENTS]; /* Threads pour clients*/
void * fctThread(void * param);
char * getThreadIdentity();
int hSocketConnectee[NB_MAX_CLIENTS]; /* Sockets pour clients*/

int main()
{
    int hSocketEcoute, /* Socket d'ecoute pour l'attente */
    hSocketService;
    int i,j, /* variables d'iteration */
    retRecv; /* Code de retour dun recv */
    struct hostent * infosHost; /*Infos sur le host : pour gethostbyname */
    struct in_addr adresseIP; /* Adresse Internet au format reseau */
    struct sockaddr_in adresseSocket;
    int tailleSockaddr_in;
    int ret, * retThread;
    char msgServeur[MAXSTRING];

/* 1. Initialisations */
    puts("* Thread principal serveur demarre *");
    printf("identite = %d.%u\n", getpid(), pthread_self());
    pthread_mutex_init(&mutexIndiceCourant, NULL);
    pthread_cond_init(&condIndiceCourant, NULL);
    /* Si la socket n'est pas utilisee, le descripteur est a -1 */
    for (i=0; i<NB_MAX_CLIENTS; i++) hSocketConnectee[i] = -1;

	pressEnter();

/* 2. Creation de la socket d'ecoute */
    hSocketEcoute = socket(AF_INET,SOCK_STREAM,0);
    if (hSocketEcoute == -1)
    {
        printf("Erreur de creation de la socket %d\n", errno);
        exit(1);
    }
    else printf("Creation de la socket OK\n");

	pressEnter();

/* 3. Acquisition des informations sur l'ordinateur local */
    if ( (infosHost = gethostbyname("copernic"))==0)
    {
        printf("Erreur d'acquisition d'infos sur le host %d\n", errno);
        exit(1);
    }
    else printf("Acquisition infos host OK\n");

    memcpy(&adresseIP, infosHost->h_addr, infosHost->h_length);
    printf("Adresse IP = %s\n",inet_ntoa(adresseIP));

	pressEnter();

/* 4. Préparation de la structure sockaddr_in */
    memset(&adresseSocket, 0, sizeof(struct sockaddr_in));
    adresseSocket.sin_family = AF_INET;
    adresseSocket.sin_port = htons(PORT);
    memcpy(&adresseSocket.sin_addr, infosHost->h_addr, infosHost->h_length);

	pressEnter();

/* 5. Le système prend connaissance de l'adresse et du port de la socket */
    if (bind(hSocketEcoute, (struct sockaddr *)&adresseSocket, sizeof(struct sockaddr_in)) == -1)
    {
        printf("Erreur sur le bind de la socket %d\n", errno);
        exit(1);
    }
    else printf("Bind adresse et port socket OK\n");

	pressEnter();

/* 6. Lancement des threads */
    for (i=0; i<NB_MAX_CLIENTS; i++)
    {
        ret = pthread_create(&threadHandle[i],NULL,fctThread, (void*)i);
        printf("Thread secondaire %d lance !\n", i);
        ret = pthread_detach(threadHandle[i]);
    }
    
	pressEnter();

    do
    {

    /* 7. Mise a l'ecoute d'une requete de connexion */
        puts("Thread principal : en attente d'une connexion");
        if (listen(hSocketEcoute,SOMAXCONN) == -1)
        {
            printf("Erreur sur lel isten de la socket %d\n", errno);
            close(hSocketEcoute); /* Fermeture de la socket */
            exit(1);
        }
        else printf("Listen socket OK\n");

		pressEnter();

    /* 8. Acceptation d'une connexion */
        tailleSockaddr_in = sizeof(struct sockaddr_in);
        if ((hSocketService = accept(hSocketEcoute, (struct sockaddr *)&adresseSocket, &tailleSockaddr_in) ) == -1)
        {
            printf("Erreur sur l'accept de la socket %d\n", errno);
            close(hSocketEcoute); /* Fermeture de la socket */
            exit(1);
        }
        else printf("Accept socket OK\n");

		pressEnter();

    /* 9. Recherche d'une socket connectee libre */
        printf("Recherche d'une socket connecteee libre ...\n");
        for (j=0; j<NB_MAX_CLIENTS && hSocketConnectee[j] !=-1; j++);
        if (j == NB_MAX_CLIENTS)
        {
            printf("Plus de connexion disponible\n");
            sprintf(msgServeur,DOC);
            if (send(hSocketService, msgServeur, MAXSTRING, 0) == -1)
            {
                printf("Erreur sur le send de refus%d\n", errno);
                close(hSocketService); /* Fermeture de la socket */
                exit(1);
            }
            else printf("Send socket refusee OK");
            
            close(hSocketService); /* Fermeture de la socket */
        }
        else
        {
            /* Il y a une connexion de libre */
            printf("Connexion sur la socket num. %d\n", j);
            pthread_mutex_lock(&mutexIndiceCourant);
            hSocketConnectee[j] = hSocketService;
            indiceCourant=j;
            pthread_mutex_unlock(&mutexIndiceCourant);
            pthread_cond_signal(&condIndiceCourant);
        }
    }while (1);

	pressEnter();

/* 10. Fermeture de la socket d'ecoute */
    close(hSocketEcoute); /* Fermeture de la socket */
    printf("Socket serveur fermee\n");
    puts("Fin du thread principal");
    return 0;
}

/*----------------------------------------------------------------*/
/*                       fctThread()                      */
/*----------------------------------------------------------------*/
    void * fctThread (void *param)
    {
        char * nomCli, *buf = (char*)malloc(100);
        char msgClient[MAXSTRING], msgServeur[MAXSTRING];
        int vr = (int)(param), finDialogue=0, i, iCliTraite;
        int temps, retRecv;
        char * numThr = getThreadIdentity();
        int hSocketServ;

        while (1)
        {
        /* 1. Attente d'un client à traiter */
            pthread_mutex_lock(&mutexIndiceCourant);
            while (indiceCourant == -1)
            pthread_cond_wait(&condIndiceCourant, &mutexIndiceCourant);
            iCliTraite = indiceCourant; indiceCourant=-1;
            hSocketServ = hSocketConnectee[iCliTraite];
            pthread_mutex_unlock(&mutexIndiceCourant);
            sprintf(buf,"Je m'occupe du numero %d ...", iCliTraite);affThread(numThr, buf);
        
        /* 2. Dialogue thread-client */
            finDialogue=0;
            do
            {
                if ((retRecv=recv(hSocketServ, msgClient, MAXSTRING,0)) == -1)
                {
                    printf("Erreur sur le recv de la socket connectee : %d\n", errno);
                    close (hSocketServ); exit(1);
                }
                else if (retRecv==0)
                {
                    sprintf(buf,"Le client est parti !!!"); affThread(numThr, buf);
                    finDialogue=1;
                    break;
                }
                else
                {
                    sprintf(buf,"Message recu = %s\n", msgClient);
                    affThread(numThr, buf);
                }
                if (strcmp(msgClient, EOC)==0)
                {
                    finDialogue=1; break;
                }
                
                sprintf(msgServeur,"ACK pour votre message : <%s>", msgClient);
                
                if (send(hSocketServ, msgServeur, MAXSTRING, 0) == -1)
                {
                    printf("Erreur sur le send de la socket %d\n", errno);
                    close(hSocketServ); /* Fermeture de la socket */
                    exit(1);
                }
                else
                {
                    sprintf(buf,"Send socket connectee OK\n");
                    affThread(numThr, buf);
                }
            }while (!finDialogue);
        
        /* 3. Fin de traitement */
            pthread_mutex_lock(&mutexIndiceCourant);
            hSocketConnectee[iCliTraite]=-1;
            pthread_mutex_unlock(&mutexIndiceCourant);
        }

        close (hSocketServ);
        return (void *)vr;
    }

/*----------------------------------------------------------------*/
/*                       getThreadIdentity()                      */
/*----------------------------------------------------------------*/
    char * getThreadIdentity()
    {
        unsigned long numSequence;
        char *buf = (char *)malloc(30);
        numSequence = pthread_getsequence_np( pthread_self( ) );
        sprintf(buf, "%d.%u", getpid(), numSequence);
        return buf;
    }

/*----------------------------------------------------------------*/
/*                        	  pressEnter()    	                  */
/*----------------------------------------------------------------*/

	void pressEnter()
	{
		printf("Press enter to continue...");
		getchar();
	}

