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
#include "socketLib.h"
#include "CMMP.h"

#define NB_MAX_CLIENTS 2 /* Nombre maximum de clients connectes */
#define EOC "END_OF_CONNEXION"
#define DOC "DENY_OF_CONNEXION"
#define PORT 50000 /* Port d'ecoute de la socket serveur */
#define MAXSTRING 100 /* Longueur des messages */
#define affThread(num, msg) printf("th_%s> %s\n", num, msg)
#define FILELOG "login.csv"
#define FILEPARC "FICH_PARC"
#define SERVEURCONF "serveur.conf"

pthread_mutex_t mutexIndiceCourant;
pthread_cond_t condIndiceCourant;
int indiceCourant=-1;
pthread_t threadHandle[NB_MAX_CLIENTS]; /* Threads pour clients*/
void * fctThread(void * param);
char * getThreadIdentity();

int hSocketConnectee[NB_MAX_CLIENTS]; /* Sockets pour clients*/

/* My function */
int fctFile(char *nomFile);
void openConfig();
char * searchConfig(char *param, FILE *fp);
void createLogin();
void createFichParc();
char * checkCommande(char *msg);
void pressEnter(void);
char * authentification(char *msg);
char * createContainer(char *msg);
char * container(char *msg);
void displayContainer();

/* My variables */
char * fileLog;
char * fileParc;
char * sepCsv;
char * port;

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

/* Ouverture du fichier de configuration */
    openConfig();

/* Ouverture et/ou création du fichier login.csv & FICH_PARC */
	ret = fctFile(FILELOG);
	if(ret != 0)
	{
		puts("Création du fichier.");
		createLogin();
	}
	
	ret = fctFile(FILEPARC);
	if(ret != 0)
	{
		puts("Création du fichier.");
		createFichParc();
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

                ret = checkCommande(msgClient);

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

/*----------------------------------------------------------------*/
/*             	               openConfig()    	                  */
/*----------------------------------------------------------------*/

	void openConfig()
	{
		FILE *fp;

		fp = fopen(SERVEURCONF, "r");
		
		if(fp == (FILE*) NULL)
		{
			printf("Le fichier %s n'existe pas.\n", SERVEURCONF);
			exit(1);
		}
		else {
			printf("Ouverture du fichier conf.\n");
			
            //port = (char *)malloc(MAXSTRING);
			//port = searchConfig("PORT_SERVEUR", fp);
            //fileLog = (char *)malloc(MAXSTRING);
            //fileLog = searchConfig("FILELOG", fp);
			// fileParc = searchConfig("FILEPARC", fp);
			// sepCsv = searchConfig("SEP_CSV", fp);

			//printf("Port : %s\nLog : %s\nParc : %s\nCSV : %s\n", port, fileLog, fileParc, sepCsv);

			//pressEnter();

			//fclose(fp);
		}
	}

/*----------------------------------------------------------------*/
/*             	             searchConfig()    	                  */
/*----------------------------------------------------------------*/

	char * searchConfig(char *param, FILE *fp)
	{
		char tmp[MAXSTRING] = "";
		const char s[2] = "=";
        char *token;
		char * ret = (char *)malloc(MAXSTRING);
        int find = 0;

		while(fgets(tmp, MAXSTRING, fp) != NULL && find != 1)
        {   
            printf("%s\n", tmp);

            token = strtok(tmp, s);

	        while(token != NULL) 
            {
                printf("%s\n", token);
	        	if(strcmp(token, param) == 0)
	        	{
	        		token = strtok(NULL, s);
	        		//printf("La valeur est : %s\n", token);
                    find = 1;
                    ret = token;
                    printf("La valeur est : %s\n", ret);
                    break;
	        	}
	            token = strtok(NULL, s);
	        }
        }
        return ret;
	}

/*----------------------------------------------------------------*/
/*             	               fctFile()    	                  */
/*----------------------------------------------------------------*/

	int fctFile(char *nomFile)
	{
		FILE *fp;
		
		fp = fopen(nomFile, "r");
		
		if(fp == (FILE*) NULL)
		{
			printf("Le fichier %s n'existe pas.\n", nomFile);
			return 1;
		}
		else {
			return 0;
		}
	}

/*----------------------------------------------------------------*/
/*             	            createLogin()    	                  */
/*----------------------------------------------------------------*/

	void createLogin()
	{
		FILE *loginFile;
		char log[] = "root;root\r\n";
		loginFile = fopen(FILELOG, "a");

		//Ajout login dans fichier
		fputs(log, loginFile);
		
		fclose(loginFile);
	}

/*----------------------------------------------------------------*/
/*             	          createFichParc()    	                  */
/*----------------------------------------------------------------*/

	void createFichParc()
	{
		FILE *fichParcFile;

		fichParcFile = fopen(FILEPARC, "wb");
		
		fclose(fichParcFile);
	}

/*----------------------------------------------------------------*/
/*                         checkCommande()                        */
/*----------------------------------------------------------------*/

    char * checkCommande(char *msg)
    {
        char * ret = (char *)malloc(MAXSTRING);
        switch(msg[0])
        {
            case '0':
                ret = authentification(msg);
                break;

            case '1':
                ret = createContainer(msg);
                break;

            case '2':
                ret = container(msg);
                break;

            case '6':
                ret = authentification(msg);
                if(strcmp(ret, "true") == 0)
                    ret = EOC;
                break;
            case '8':
            	displayContainer();
            	ret ="OK";
            	break;
        }
        return ret;
    }

/*----------------------------------------------------------------*/
/*                         authentification()                        */
/*----------------------------------------------------------------*/

    char * authentification(char *msg)
    {
        FILE *fp;
        char identifiant[MAXSTRING] = "";
        char *find = (char *)malloc(MAXSTRING);
        char **param = NULL;
        char **id = NULL;

        fp = fopen(FILELOG, "r");
        
        if(fp == (FILE*) NULL)
        {
            printf("Le fichier %s n'existe pas.\n", FILELOG);
            exit(1);
        }
        else {   

            param = tokenizer(msg);

            while(fgets(identifiant, MAXSTRING, fp) != NULL)
            {   
                id = NULL; 
                id = tokenizer(identifiant);
                if(strcmp(id[0], param[1]) == 0)
                {
                    if(strcmp(id[1], param[2]) == 0)
                    {
                        find = "true";
                        break;
                    }
                    find = "false";
                }
                else {
                    find = "false";
                }
            }
            return find;
            fclose(fp);
        }
    }

/*----------------------------------------------------------------*/
/*                       createContainer()                        */
/*----------------------------------------------------------------*/

    char * createContainer(char *msg)
    {
        FILE *fp;

        char * ret = (char *)malloc(MAXSTRING);
        char **param = NULL;
        char random[MAXSTRING];

        param = tokenizer(msg);

        Container* newContainer;
        newContainer = malloc(sizeof(Container));

        strcpy(newContainer->idContainer, param[2]);
        sprintf(random, "%d" , rand()%50);
        strcpy(newContainer->coordonnees, random);
        strcat(newContainer->coordonnees, ",");
        sprintf(random, "%d" , rand()%50);
        strcat(newContainer->coordonnees, random);

        fp = fopen(FILEPARC, "a+b");


        //Ajout login dans fichier        
        if(fwrite(newContainer, sizeof(Container), 1, fp) != 0)  
        {
            printf("Le container a bien ete ajoute !\n");
            strcpy(ret, param[0]);
            strcat(ret, "#");
            strcat(ret, newContainer->coordonnees);
            strcat(ret, ";");
            strcat(ret, newContainer->idContainer);
        }
        else 
            printf("Erreur d'ecriture dans le fichier !\n"); 

        free(newContainer);
        fclose(fp);

        return ret;
    }

/*----------------------------------------------------------------*/
/*                      	 container()                          */
/*----------------------------------------------------------------*/

	char * container(char *msg)
	{
        FILE *fp;

        char * ret = (char *)malloc(MAXSTRING);
        char **param = NULL;
        int i = 0;

        param = tokenizer(msg);

        fp = fopen(FILEPARC, "r+b");

        Container* uploadContainer;
        uploadContainer = malloc(sizeof(Container));

        while(fread(uploadContainer, sizeof(Container), 1, fp))
        {
        	i++;
            if(strcmp(uploadContainer->idContainer, param[2]) == 0)
            {
                printf("Trouve\n");
                break;
            }
        }


        uploadContainer->poids = 125;

        i--;
        fseek(fp, i*sizeof(Container), SEEK_SET);
        fwrite(uploadContainer, sizeof(Container), 1, fp);

        printf("Container upload\n");

        free(uploadContainer);
        fclose(fp);

        ret = "OK";
        return ret;
    }

/*----------------------------------------------------------------*/
/*                      displayContainer()                        */
/*----------------------------------------------------------------*/

	void displayContainer()
	{
		FILE *fp;

        fp = fopen(FILEPARC, "rb");

        Container* container;
        container = malloc(sizeof(Container));
        
		printf("Afficher container\n");

        while(fread(container, sizeof(Container), 1, fp))
        {

            printf("%s - %s - %d\n", container->idContainer, container->coordonnees, container->poids);
        }

        free(container);
        fclose(fp);
	}