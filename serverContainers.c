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

#define PORT 50000
#define MAXSTRING 100

void pressEnter(void);

int main ()
{
	int hSocketEcoute, hSocketService;
	struct hostent * infosHost;
	struct in_addr adresseIP;
	struct sockaddr_in adresseSocket;
	int tailleSockaddr_in;

	char msgClient[MAXSTRING], msgServeur[MAXSTRING];
	int nbreRecv;

/* 1. Création de la socket */
	hSocketEcoute= socket(AF_INET, SOCK_STREAM, 0);
	if (hSocketEcoute== -1)
	{
		printf("Erreur de creation de la socket %d\n", errno);
		exit(1);
	}
	else 
		printf("Creation de la socket OK\n");

/* 2. Acquisition des informations sur l'ordinateur local */
	if ( (infosHost = gethostbyname("solaris11DM2017"))==0)
	{
		printf("Erreur d'acquisition d'infos sur le host %d\n", errno);
		exit(1);
	}
	else 
		printf("Acquisition infos host OK\n");
	
	memcpy(&adresseIP, infosHost->h_addr, infosHost->h_length);
	printf("Adresse IP = %s\n",inet_ntoa(adresseIP));
	/* Conversion de l'adresse contenue dans le structure in_addr
	en une chaine comprehensible */

/* 3. Préparation de la structure sockaddr_in */
	memset(&adresseSocket, 0, sizeof(struct sockaddr_in));
	adresseSocket.sin_family = AF_INET; /* Domaine */
	adresseSocket.sin_port = htons(PORT);
	/* conversion numéro de port au format réseau _*/
	memcpy(&adresseSocket.sin_addr, infosHost->h_addr,infosHost->h_length);

/* 4. Le système prend connaissance de l'adresse et du port de la socket */
	if (bind(hSocketEcoute, (struct sockaddr *)&adresseSocket,
	sizeof(struct sockaddr_in)) == -1)
	{
		printf("Erreur sur le bind de la socket %d\n", errno);
		exit(1);
	}
	else 
		printf("Bind adresse et port socket OK\n");

/* 5. Mise a l'ecoute d'une requete de connexion */
	if (listen(hSocketEcoute,SOMAXCONN) == -1)
	{
		printf("Erreur sur lel isten de la socket %d\n", errno);
		close(hSocketEcoute); /* Fermeture de la socket */
		exit(1);
	}
	else 
		printf("Listen socket OK\n");

/* 6. Acceptation d'une connexion */
	tailleSockaddr_in = sizeof(struct sockaddr_in);
	if ( (hSocketService =	accept(hSocketEcoute, (struct sockaddr *)&adresseSocket, &tailleSockaddr_in) ) == -1)
	{
		printf("Erreur sur l'accept de la socket %d\n", errno);
		close(hSocketEcoute); exit(1);
	}
	else 
		printf("Accept socket OK\n");

/* 7.Reception d'un message client */
	if ((nbreRecv = recv(hSocketService, msgClient, MAXSTRING, 0)) == -1) /* pas message urgent */
	{
		printf("Erreur sur le recv de la socket %d\n", errno);
		close(hSocketEcoute); /* Fermeture de la socket */
		close(hSocketService); /* Fermeture de la socket */
		exit(1);
	}
	else 
		printf("Recv socket OK\n");
	
	msgClient[nbreRecv]=0;
	printf("Message recu = %s\n", msgClient);

/* 8. Envoi de l'ACK du serveur au client */
	sprintf(msgServeur,"ACK pour votre message : <%s>", msgClient);
	if (send(hSocketService, msgServeur, MAXSTRING, 0) == -1)
	{
		printf("Erreur sur le send de la socket %d\n", errno);
		close(hSocketEcoute); /* Fermeture de la socket */
		close(hSocketService); /* Fermeture de la socket */
		exit(1);
	}
	else 
		printf("Send socket OK\n");

/* 9. Reception d'un second message client */
	if ((nbreRecv = recv(hSocketService, msgClient, MAXSTRING, 0)) == -1)
	/* pas message urgent */
	{
		printf("Erreur sur le recv de la socket %d\n", errno);
		close(hSocketEcoute); /* Fermeture de la socket */
		close(hSocketService); /* Fermeture de la socket */
		exit(1);
	}
	else 
		printf("Recv socket OK\n");

	msgClient[nbreRecv]=0;
	printf("Message recu = %s\n", msgClient);

/* 10. Envoi de l'ACK du serveur au client */
	sprintf(msgServeur,"ACK pour votre message : <%s>", msgClient);
	if (send(hSocketService, msgServeur, MAXSTRING, 0) == -1)
	{
		printf("Erreur sur le send de la socket %d\n", errno);
		close(hSocketEcoute); /* Fermeture de la socket */
		close(hSocketService); /* Fermeture de la socket */
		exit(1);
	}
	else 
		printf("Send socket OK\n");

/* 11. Fermeture des sockets */
	close(hSocketService); /* Fermeture de la socket */
	printf("Socket connectee au client fermee\n");
	close(hSocketEcoute); /* Fermeture de la socket */
	printf("Socket serveur fermee\n");

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
