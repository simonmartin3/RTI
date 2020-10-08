#include "CMMP.h"

#define MAXSTRING 		100
#define LOGIN 			0
#define INPUT_TRUCK 	1
#define INPUT_DONE 		2
#define OUTPUT_READY 	3
#define OUTPUT_ONE		4
#define OUTPUT_DONE		5
#define LOGOUT 			6

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
	strcat(message, "\r\n");

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
	strcat(message, "\r\n");

	return message;
}

char * inputTruck()
{
	int type = INPUT_TRUCK;
	char *message = (char *)malloc(MAXSTRING);
	char immatriculation[20], idContainer[20];

	puts("Entrer l'immatriculation :");
	scanf("%s", immatriculation);
	
	fflush(stdin);
	
	puts("Entrer l'identifiant du container' :");
	scanf("%s", idContainer);
	
	fflush(stdin);

	sprintf(message, "%d", type);

	strcat(message, "#");
	strcat(message, immatriculation);
	strcat(message, ";");
	strcat(message, idContainer);

	return message;
}

char * inputDone()
{
	int type = INPUT_DONE;
	char *message = (char *)malloc(MAXSTRING);

	sprintf(message, "%d", type);

	strcat(message, "#");
	strcat(message, "125.6");

	return message;
}

char * tokenizer(char *msg)
{
	char *param[][MAXSTRING] = {};
	int i;
	const char s[2] = "#;";
    char *token;

    token = strtok(msg, s);

	for(i=0; token != NULL; i++)
	{
		strcpy(param[i], token);
		printf("%s\n", param[i]);
		token = strtok(NULL, s);
	}

	return param;
}

void pressEnter()
{
	printf("Press enter to continue...");
	getchar();
}