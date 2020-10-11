#include "CMMP.h"

#define MAXSTRING 		100
#define LOGIN 			0
#define INPUT_TRUCK 	1
#define INPUT_DONE 		2
#define OUTPUT_READY 	3
#define OUTPUT_ONE		4
#define OUTPUT_DONE		5
#define LOGOUT 			6

#define SERVEURCONF "serveur.conf"

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
	char random[50];

	sprintf(message, "%d", type);

	strcat(message, "#");

	sprintf(random, "%d" , rand()%250);

	strcat(message, random);

	return message;
}

char * outputReady()
{
	int type = OUTPUT_READY;
	char *message = (char *)malloc(MAXSTRING);
	char idVehicule[20], dest[50], capacite[10];
	int tmp;

	puts("Entrer l'identifiant :");
	scanf("%s", idVehicule);
	
	fflush(stdin);
	
	puts("Entrer la destination :");
	scanf("%s", dest);

	puts("Entrer la capacite :");
	scanf("%d", &tmp);
	
	fflush(stdin);

	sprintf(capacite, "%d", tmp);
	sprintf(message, "%d", type);

	strcat(message, "#");
	strcat(message, idVehicule);
	strcat(message, ";");
	strcat(message, dest);
	strcat(message, ";");
	strcat(message, capacite);

	return message;
}


char ** tokenizer(char *msg, char *ct)
{
	char **tab = NULL;
   	
   	if (msg != NULL && ct != NULL)
   	{
      	int i;
      	char *cs = NULL;
      	size_t size = 1;

		/* (1) */
      	for (i = 0; (cs = strtok (msg, ct)); i++)
      	{
         	if (size <= i + 1)
         	{
	            void *tmp = NULL;

				/* (2) */
	            size <<= 1;
	            tmp = realloc (tab, sizeof (*tab) * size);
	            if (tmp != NULL)
	            {
	               tab = tmp;
	            }
	            else
	            {
	               fprintf (stderr, "Memoire insuffisante\n");
	               free (tab);
	               tab = NULL;
	               exit (EXIT_FAILURE);
	            }
         	}

		/* (3) */
        tab[i] = cs;
        msg = NULL;
    	}
    
    tab[i] = NULL;
  	
  	}	
	
	return tab;
}

void pressEnter()
{
	printf("Press enter to continue...");
	getchar();
}

/*----------------------------------------------------------------*/
/*             	             searchConfig()    	                  */
/*----------------------------------------------------------------*/

	char * searchConfig(char *config)
	{
		FILE *fp;

		char tmp[MAXSTRING] = "";
		char * ret = (char *)malloc(MAXSTRING);
        char **param = NULL;

        fp = fopen(SERVEURCONF, "r");
		
		if(fp == (FILE*) NULL)
		{
			printf("Le fichier %s n'existe pas.\n", SERVEURCONF);
			exit(1);
		}
		else {
			printf("Ouverture du fichier conf.\n");

			while(fgets(tmp, MAXSTRING, fp) != NULL)
	        {   
	            param = tokenizer(tmp, "=");

	            if(strcmp(param[0], config) == 0)
	            {
	                ret = param[1];
	                break;
	            }
	        }
		}
        return ret;
	}