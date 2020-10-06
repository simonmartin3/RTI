#include "CMMP.h"

#define MAXSTRING 		100
#define LOGIN 			1
#define INPUT_TRUCK 	2
#define INPUT_DONE 		3
#define OUTPUT_READY 	4
#define OUTPUT_ONE		5
#define OUTPUT_DONE		6
#define LOGOUT 			7

char * login()
{
	int type = LOGIN;
	char *message = (char *)malloc(MAXSTRING);
	char user[30], pass[30];

	puts("Entrer l'user :");
	scanf("%s", user);
	
	fflush(stdin);
	
	puts("Entrer le password :");
	scanf("%s", pass);
	
	fflush(stdin);

	sprintf(message, "%d", type);

	strcat(message, "#");
	strcat(message, user);
	strcat(message, ";");
	strcat(message, pass);

	return message;
}

char * logout()
{
	int type = LOGOUT;
	char *message = (char *)malloc(MAXSTRING);
	char user[30], pass[30];

	puts("Entrer l'user :");
	scanf("%s", user);
	
	fflush(stdin);
	
	puts("Entrer le password :");
	scanf("%s", pass);
	
	fflush(stdin);

	sprintf(message, "%d", type);

	strcat(message, "#");
	strcat(message, user);
	strcat(message, ";");
	strcat(message, pass);

	return message;
}
