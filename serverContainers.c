#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <time.h>
#include "socketLib.h"
#include "utilLib.h"
#include "CMMP.h"

#define NB_MAX_CLIENTS 2 /* Nombre maximum de clients connectes */
#define EOC "END_OF_CONNEXION"
#define DOC "DENY_OF_CONNEXION"
#define MAXSTRING 100 /* Longueur des messages */
#define OK "true"
#define FAIL "false"
#define affThread(num, msg) printf("th_%s> %s\n", num, msg)

int PORT;
char FILELOG[20];
char FILEPARC[20];
char SEP_CSV[10];
char FILEVEHICULE[30];
int hSocketServ;

pthread_mutex_t mutexIndiceCourant;
pthread_cond_t condIndiceCourant;
int indiceCourant=-1;
pthread_t threadHandle[NB_MAX_CLIENTS]; /* Threads pour clients*/
void * fctThread(void * param);
char * getThreadIdentity();

int hSocketConnectee[NB_MAX_CLIENTS]; /* Sockets pour clients*/


int main ()
{
	int hSocketEcoute, hSocketService;
	//int i,j,retRecv;
	int i,j;
	//struct hostent * infosHost;
	//struct in_addr adresseIP;
	struct sockaddr_in adresseSocket;
	//int tailleSockaddr_in;
	//int ret, * retThread;
	int ret;
    char msgServeur[MAXSTRING];
    char tmp[20];

/* Ouverture du fichier de configuration */

    memcpy(tmp, searchConfig("PORT_SERVEUR"), sizeof(tmp));
    PORT = atoi(tmp);

    memcpy(FILELOG, searchConfig("FILELOG"), sizeof(FILELOG));
    FILELOG[strlen(FILELOG)-1] = '\0';

    memcpy(FILEPARC, searchConfig("FILEPARC"), sizeof(FILEPARC));
    FILEPARC[strlen(FILEPARC)-1] = '\0';

    memcpy(FILEVEHICULE, searchConfig("FILEVEHICULE"), sizeof(FILEVEHICULE));
    FILEVEHICULE[strlen(FILEVEHICULE)-1] = '\0';

    memcpy(SEP_CSV, searchConfig("SEP_CSV"), sizeof(SEP_CSV));
    SEP_CSV[strlen(SEP_CSV)-1] = '\0';

/* Ouverture et/ou création du fichier login.csv & FICH_PARC */
    ret = fctFile(FILELOG);
    if(ret != 0)
    {
        puts("Création du fichier.");
        createLogin(FILELOG);
    }
    
    ret = fctFile(FILEPARC);
    if(ret != 0)
    {
        puts("Création du fichier.");
        createFich(FILEPARC);
    }

    ret = fctFile(FILEVEHICULE);
    if(ret != 0)
    {
        puts("Création du fichier.");
        createFich(FILEVEHICULE);
    }

/* 1. Initialisations */
    puts("* Thread principal serveur demarre *");
    printf("identite = %d.%u\n", getpid(), pthread_self());
    pthread_mutex_init(&mutexIndiceCourant, NULL);
    pthread_cond_init(&condIndiceCourant, NULL);
    /* Si la socket n'est pas utilisee, le descripteur est a -1 */
    for (i=0; i<NB_MAX_CLIENTS; i++) 
		hSocketConnectee[i] = -1;

	hSocketEcoute = SocketInit(&adresseSocket, "solaris11DM2017", PORT);

/* 4. Le système prend connaissance de l'adresse et du port de la socket */
	if (bind(hSocketEcoute, (struct sockaddr *)&adresseSocket, sizeof(struct sockaddr_in)) == -1)
	{
		printf("Erreur sur le bind de la socket %d\n", errno);
		exit(1);
	}
	else 
		printf("Bind adresse et port socket OK\n");

/* 6. Lancement des threads */
    for (i=0; i<NB_MAX_CLIENTS; i++)
    {
/*        ret = pthread_create(&threadHandle[i],NULL,fctThread, (void*)i);*/
        ret = pthread_create(&threadHandle[i],NULL,fctThread, &i);
        printf("Thread secondaire %d lance !\n", i);
        ret = pthread_detach(threadHandle[i]);
    }
    

    do
    {

    ListenAccept(&hSocketService, &hSocketEcoute, (struct sockaddr *)&adresseSocket);

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
        char *buf = (char*)malloc(100);
        char msgClient[MAXSTRING], msgServeur[MAXSTRING];
        int vr = (int)(param), finDialogue=0, iCliTraite;
        int retRecv;
        char * numThr = getThreadIdentity();
        char * ret = (char *)malloc(MAXSTRING);
        Message msgRecv;
        Container* container;
        Vehicule *newVehicule;
        
        FILE *fp;
        char **tmp = NULL;

        while (1)
        {
        /* 1. Attente d'un client à traiter */
            pthread_mutex_lock(&mutexIndiceCourant);
            while (indiceCourant == -1)
            pthread_cond_wait(&condIndiceCourant, &mutexIndiceCourant);
            iCliTraite = indiceCourant; indiceCourant=-1;
            hSocketServ = hSocketConnectee[iCliTraite];
            pthread_mutex_unlock(&mutexIndiceCourant);
            sprintf(buf,"Je m'occupe du numero %d ...", iCliTraite);
			affThread(numThr, buf);
        
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
                    sprintf(buf,"Le client est parti !!!"); 
                    affThread(numThr, buf);
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
                

                memcpy(&msgRecv, msgClient, sizeof(struct Message));

                switch(msgRecv.typeReq)
		        {
		            case 0 :
		                ret = authentification(msgRecv.msg, FILELOG);
		                break;

		            case 1:
		                ret = createContainer(msgRecv.msg, FILEPARC);
                        break;

                    case 2:
                        ret = uploadContainer(msgRecv.msg, FILEPARC);
                        break;

		            case 3:
                        
                        container = malloc(sizeof(Container));
                        newVehicule = malloc(sizeof(Vehicule));

                        tmp = tokenizer(msgRecv.msg, ";");

                        strcpy(newVehicule->typeVehicule, tmp[0]);
                        strcpy(newVehicule->idVehicule, tmp[1]);
                        strcpy(newVehicule->destination, tmp[2]);
                        newVehicule->capacite = atoi(tmp[3]);   

                        fp = fopen(FILEVEHICULE, "a+b");

                        //Ajout login dans fichier        
                        if(fwrite(newVehicule, sizeof(Vehicule), 1, fp) != 0)  
                        {
                            printf("Le container a bien ete ajoute !\n");
                            ret = OK;
                        }
                        else 
                        {    
                            printf("Erreur d'ecriture dans le fichier !\n"); 
                            ret = FAIL;
                        }
                        
                        fclose(fp);

                        fp = fopen(FILEPARC, "r+b");
                        fseek(fp, 0L, SEEK_END);
                        rewind(fp);

                        while(fread(container, sizeof(Container), 1, fp))
                        {
                            if(strcmp(container->typeRetour, tmp[0]) == 0)
                            {
                                if(strcmp(container->destination, tmp[2]) == 0)
                                {
                                    ret = toString(container);
                                    sprintf(msgServeur,"%s", ret);
                                    if (send(hSocketServ, msgServeur, MAXSTRING, 0) == -1)
                                    {
                                        printf("Erreur sur le send de la socket %d\n", errno);
                                        close(hSocketServ); /* Fermeture de la socket */
                                        exit(1);
                                    }
                                    else
                                    {
                                        sprintf(buf,"Send container list\n");
                                        affThread(numThr, buf);
                                    }
                                }
                            }
                        }
                        ret = OK;
                        sprintf(msgServeur,"%s", ret);
                
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
		                break;

                    case 4 :
                            ret = moveContainer(msgRecv.msg, FILEPARC, FILEVEHICULE);
                        break;

                    case 5 :
                            printf("Le vehicule %s est plein\n", msgRecv.msg);
                            ret = OK;
                        break;

		            case 6 :
		                ret = authentification(msgRecv.msg, FILELOG);
		                if(strcmp(ret, OK) == 0)
		                    ret = EOC;
		                break;
		            case 7:
		            	ret = displayContainer(FILEPARC);
		            	break;

                    case 8:
                        ret = displayVehicule(FILEVEHICULE);
                        break;
		        }
                
                sprintf(msgServeur,"%s", ret);
                
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
        //numSequence = pthread_getsequence_np( pthread_self( ) );
		numSequence = 2;
        sprintf(buf, "%d.%lu", getpid(), numSequence);
        return buf;
    }