#include "socketLib.h"

int SocketInit(struct sockaddr_in* si, char* hostname, unsigned long port)
{ 
	struct hostent *infosHost;
	struct in_addr adresseIP;
	int hSocketEcoute;

	hSocketEcoute = socket(AF_INET, SOCK_STREAM, 0);
	if(hSocketEcoute == -1)
	{
		printf("Erreur de creation de la socket %d\n ", errno);
		exit(1);
	}
	else
	{
		printf("Creation de la socket OK\n");
	}

	/* Acquisition des informations sur l'ordinateur dont le nom est hostname */
	if((infosHost = gethostbyname(hostname))==0)
	{
		printf("Erreur d'acquisition d'infos sur le host %d\n", errno);
		exit(1);
	}
	else
	{
		printf("Acquisition infos host OK\n");
	}

	memcpy(&adresseIP, infosHost->h_addr, infosHost->h_length);
	inet_aton("192.168.29.131", &adresseIP);
	printf("adresseIP = %s\n", inet_ntoa(adresseIP));

	/* Préparation de la sockaddre_in*/
	memset(si, 0, sizeof(struct sockaddr_in));
	si->sin_family = AF_INET;
	si->sin_port = htons(PORT);
	memcpy(&si->sin_addr, infosHost->h_addr, infosHost->h_length);
		
	return hSocketEcoute;
}

int SocketInitWithIpAddr(struct sockaddr_in* si, char* hostname, unsigned long port, char* ipAddr)
{ 
	struct hostent *infosHost;
	struct in_addr adresseIP;
	int hSocketEcoute;

	hSocketEcoute = socket(AF_INET, SOCK_STREAM, 0);
	if(hSocketEcoute == -1)
	{
		printf("Erreur de creation de la socket %d\n ", errno);
		exit(1);
	}
	else
	{
		printf("Creation de la socket OK\n");
	}

	/* Acquisition des informations sur l'ordinateur dont le nom est hostname */
	if((infosHost = gethostbyname(hostname))==0)
	{
		printf("Erreur d'acquisition d'infos sur le host %d\n", errno);
		exit(1);
	}
	else
	{
		printf("Acquisition infos host OK\n");
	}
	inet_aton(ipAddr, infosHost->h_addr);
	
	memcpy(&adresseIP, infosHost->h_addr, infosHost->h_length);
	
	printf("adresseIP = %s\n", inet_ntoa(adresseIP));

	/* Préparation de la sockaddre_in*/
	memset(si, 0, sizeof(struct sockaddr_in));
	si->sin_family = AF_INET;
	si->sin_port = htons(port);
	printf("port = %d\n", port);
	memcpy(&si->sin_addr, infosHost->h_addr, infosHost->h_length);
		
	return hSocketEcoute;
}

void ListenAccept(int * hSocketService, int * hSocketEcoute, struct sockaddr * adresseSocket )
{
		int tailleSockaddr_in = sizeof(struct sockaddr_in);

		/* Mise à l'écoute d'une requête de connexion*/
		if(listen(*hSocketEcoute, SOMAXCONN)==-1)
		{
			printf("Erreur sur le listen de la socket %d\n", errno);
			close(hSocketEcoute);
			exit(1);
		}
		else
			printf("Listen socket OK\n");

	/* Acceptation d'une connexion*/
		
		if((*hSocketService = accept(*hSocketEcoute, adresseSocket, &tailleSockaddr_in))==-1)
		{
			printf("Erreur sur l'accept de la socket %d\n", errno);
			close(hSocketEcoute);
			exit(1);
		}
		else
			printf("Accept socket OK\n");

}