#include "CMMP.h"

#define MAXSTRING 		100
#define LOGIN 			0
#define INPUT_TRUCK 	1
#define INPUT_DONE 		2
#define OUTPUT_READY 	3
#define OUTPUT_ONE		4
#define OUTPUT_DONE		5
#define LOGOUT 			6

struct Message login()
{
	Message message;

	char *msg = (char *)malloc(MAXSTRING);
	char user[30], pass[30];

	puts("Entrer l'user :");
	scanf("%s", user);
	
	fflush(stdin);
	
	puts("Entrer le password :");
	scanf("%s", pass);
	
	fflush(stdin);

	message.typeReq = LOGIN;

	strcpy(msg, user);
	strcat(msg, ";");
	strcat(msg, pass);
	strcat(msg, "\r\n");

	strcpy(message.msg, msg);

	return message;
}

struct Message logout()
{

	Message message;

	char *msg = (char *)malloc(MAXSTRING);
	char user[30], pass[30];

	puts("Entrer l'user :");
	scanf("%s", user);
	
	fflush(stdin);
	
	puts("Entrer le password :");
	scanf("%s", pass);
	
	fflush(stdin);

	message.typeReq = LOGOUT;

	strcpy(msg, user);
	strcat(msg, ";");
	strcat(msg, pass);
	strcat(msg, "\r\n");

	strcpy(message.msg, msg);

	return message;
}

struct Message inputTruck()
{
	Message message;

	char *msg = (char *)malloc(MAXSTRING);
	char immatriculation[20], idContainer[20];

	puts("Entrer l'immatriculation :");
	scanf("%s", immatriculation);
	
	fflush(stdin);
	
	puts("Entrer l'identifiant du container' :");
	scanf("%s", idContainer);
	
	fflush(stdin);

	message.typeReq = INPUT_TRUCK;

	strcpy(msg, immatriculation);
	strcat(msg, ";");
	strcat(msg, idContainer);

	strcpy(message.msg, msg);

	return message;
}

struct Message inputDone(char * idContainer)
{
	Message message;

	char poids[50];

	sprintf(poids, "%d" , rand()%250);

	message.typeReq = INPUT_DONE;

	strcpy(message.msg, idContainer);
	strcat(message.msg, ";");
	strcat(message.msg, poids);

	return message;
}

struct Message outputReady()
{
	Message message;

	char *msg = (char *)malloc(MAXSTRING);
	char idVehicule[20], dest[50], capacite[10];
	int typeV, tmp;

	do
    {
        //system("clear");
        printf("1 - Bateau\n");
        printf("2 - Train\n");
        scanf("%d", &typeV);
        fflush(stdin);
        typeV = (int)typeV;
    }while(typeV < 1 || typeV > 2);

	puts("Entrer l'identifiant :");
	scanf("%s", idVehicule);
	
	fflush(stdin);
	
	puts("Entrer la destination :");
	scanf("%s", dest);

	puts("Entrer la capacite :");
	scanf("%d", &tmp);
	
	fflush(stdin);

	sprintf(capacite, "%d", tmp);
	
	message.typeReq = OUTPUT_READY;

	if(typeV == 1)
		strcat(msg, "Bateau");
	else
		strcat(msg, "Train");
	strcat(msg, ";");
	strcat(msg, idVehicule);
	strcat(msg, ";");
	strcat(msg, dest);
	strcat(msg, ";");
	strcat(msg, capacite);

	strcpy(message.msg, msg);

	return message;
}

struct Message outputOne()
{
	Message message;

	char *msg = (char *)malloc(MAXSTRING);
	char idVehicule[20];

	puts("Entrer l'identifiant :");
	scanf("%s", idVehicule);
	fflush(stdin);

	message.typeReq = OUTPUT_ONE;
	strcpy(msg, idVehicule);
	strcpy(message.msg, msg);
}

