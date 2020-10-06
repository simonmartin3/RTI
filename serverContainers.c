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

#define NB_MAX_CLIENTS 2 /* Nombre maximum de clients connectes */
#define EOC "END_OF_CONNEXION"
#define DOC "DENY_OF_CONNEXION"
#define PORT 50000 /* Port d'ecoute de la socket serveur */
#define MAXSTRING 100 /* Longueur des messages */
#define affThread(num, msg) printf("th_%s> %s\n", num, msg)
#define FILELOG "login.csv"
#define FILEPARC "FICH_PARC"

pthread_mutex_t mutexIndiceCourant;
pthread_cond_t condIndiceCourant;
int indiceCourant=-1;
pthread_t threadHandle[NB_MAX_CLIENTS]; /* Threads pour clients*/
void * fctThread(void * param);
char * getThreadIdentity();

int hSocketConnectee[NB_MAX_CLIENTS]; /* Sockets pour clients*/

/* My function */
int fctFile(char *nomFile);
void createLogin();
void createFichParc();
void checkCommande(char *msg);
void pressEnter(void);
void authentification(char *msg);

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

/* Ouverture et/ou création du fichier login.csv */
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

                checkCommande(msgClient);

                pressEnter();

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
        //numSequence = pthread_getsequence_np( pthread_self( ) );
		numSequence = 2;
        sprintf(buf, "%d.%lu", getpid(), numSequence);
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
		
		

		/*FILE *loginFile;
		char login[100] = "";
		int len;

		struct login {
			char user[30];
			char pass[30];
		};
		
		loginFile = fopen(FILELOG, "a+");

		if(login != NULL)
		{
			puts("Fichier ouvert");
	
			fseek(loginFile, 0, SEEK_END);			

			len = ftell(loginFile);
			if(len == 0)
			{
				struct login root = {"root", "root"};
				
				strcpy(login, root.user);
				strcat(login, ";");
				strcat(login, root.pass);
				strcat(login, "\n");

				//Ajout login dans fichier
				fputs(login, loginFile);
			} */
/*			else {*/
/*				fseek(loginFile, 0, SEEK_SET);*/
/*				//Lecture fichier*/
/*				fgets(login, 100, loginFile);*/
/*				char* token = strtok(login, ";");*/
/*				struct login test;*/
/*					*/
/*				strcpy(test.user,token);*/
/*				token = strtok(NULL, login);*/
/*				strcpy(test.pass,token);*/
/*				*/
/*				printf("%s-%s", test.user, test.pass);*/
/*				*/
/*			}*/
			/* fclose(loginFile);
		}
		else {
			puts("Impossible d'ouvrir le fichier login");
			exit(1);	
		} */
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

		fichParcFile = fopen(FILEPARC, "a");
		
		fclose(fichParcFile);
	}

/*----------------------------------------------------------------*/
/*                         checkCommande()                        */
/*----------------------------------------------------------------*/

    void checkCommande(char *msg)
    {
        switch(msg[0])
        {
            case '1':
                authentification(msg);
                break;
        }
    }

/*----------------------------------------------------------------*/
/*                         authentification()                        */
/*----------------------------------------------------------------*/

    void authentification(char *msg)
    {
        FILE *fp;
        char identifiant[MAXSTRING] = "";
        const char s[2] = "#";
        char *token;
        char test[MAXSTRING];

        fp = fopen(FILELOG, "r");
        
        if(fp == (FILE*) NULL)
        {
            printf("Le fichier %s n'existe pas.\n", FILELOG);
            exit(1);
        }
        else {   

            token = strtok(msg, s);

            while(token != NULL) {
                strcpy(test, token);
                token = strtok(NULL, s);
            }

            //strcat(test,"\r");

            while(fgets(identifiant, MAXSTRING, fp) != NULL)
            {    
                if(strcmp(identifiant, msg) == 0)
                {
                    printf("good\n");
                }
                else {
                    printf("%s\n", test);
                    printf("%s\n", identifiant);
                    printf("bad\n");
                }
            }
            fclose(fp);
        }
    }

