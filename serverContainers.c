/* tcpVarCond.c
-- MARTIN Simon --
*/
#include <stdio.h>
#include <stdlib.h> /* pour exit */
#include <string.h> /* pour memcpy */

#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
//#include <sys/socket.h> /* pour les types de socket */
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
void pressEnter(void);
int hSocketConnectee[NB_MAX_CLIENTS]; /* Sockets pour clients*/


int main(void)
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
    if ( (infosHost = gethostbyname("solaris11DM2017"))==0)
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

/* 5. Le système prend connaissance de l'adresse et du port de la socket */
    if (bind(hSocketEcoute, (struct sockaddr *)&adresseSocket, sizeof(struct sockaddr_in)) == -1)
    {
        printf("Erreur sur le bind de la socket %d\n", errno);
        exit(1);
    }
    else printf("Bind adresse et port socket OK\n");

	pressEnter();

    return 0;
}

/*----------------------------------------------------------------*/
/*                        	  pressEnter()    	                  */
/*----------------------------------------------------------------*/

	void pressEnter()
	{
		printf("Press enter to continue...");
		getchar();
	}

