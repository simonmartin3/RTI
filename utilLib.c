#include "utilLib.h"

#define MAXSTRING	100
#define OK "true"
#define FAIL "false"

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

	void createLogin(char * FILELOG)
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

	void createFichParc(char * FILEPARC)
	{
		FILE *fichParcFile;

		fichParcFile = fopen(FILEPARC, "wb");
		
		fclose(fichParcFile);
	}

/*----------------------------------------------------------------*/
/*                         authentification()                        */
/*----------------------------------------------------------------*/

    char * authentification(char *msg , char * FILELOG)
    {
        FILE *fp;
        char identifiant[MAXSTRING] = "";
        char *ret = (char *)malloc(MAXSTRING);
        char **param = NULL;
        char **id = NULL;

        fp = fopen(FILELOG, "r");
        
        if(fp == (FILE*) NULL)
        {
            printf("Le fichier %s n'existe pas.\n", FILELOG);
            exit(1);
        }
        else {

            param = tokenizer(msg, "-");

            while(fgets(identifiant, MAXSTRING, fp) != NULL)
            {   
                id = NULL; 
                id = tokenizer(identifiant, ";");
                if(strcmp(id[0], param[0]) == 0)
                {
                    if(strcmp(id[1], param[1]) == 0)
                    {
                        ret = OK;
                        break;
                    }
                    ret = FAIL;
                }
                else {
                    ret = FAIL;
                }
            }
            fclose(fp);
            return ret;
        }
    }

/*----------------------------------------------------------------*/
/*                       createContainer()                        */
/*----------------------------------------------------------------*/

    char * createContainer(char *msg, char * FILEPARC)
    {
        FILE *fp;

        char * ret = (char *)malloc(MAXSTRING);
        char **param = NULL;
        char random[MAXSTRING];
        time_t rawtime;
        struct tm *info;


        param = tokenizer(msg, "#;");

        Container* newContainer;
        newContainer = malloc(sizeof(Container));

        strcpy(newContainer->idContainer, param[2]);
        
        sprintf(random, "%d" , rand()%50);
        strcpy(newContainer->coordonnees, random);
        strcat(newContainer->coordonnees, ",");
        sprintf(random, "%d" , rand()%50);
        strcat(newContainer->coordonnees, random);

        newContainer->etat = 1;

        strcpy(newContainer->dateReservation, "01/10/2020");

        time(&rawtime);
        info = localtime(&rawtime);
        strftime(newContainer->dateArrivee, 80, "%d/%m/%Y",  info);

        if(rand()%2)
            strcpy(newContainer->destination, "Liege");
        else
            strcpy(newContainer->destination, "Namur");

        if(rand()%2)
            strcpy(newContainer->typeRetour, "Bateau");
        else
            strcpy(newContainer->typeRetour, "Train");        

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

	char * container(char *msg, char * FILEPARC)
	{
        FILE *fp;

        char * ret = (char *)malloc(MAXSTRING);
        char **param = NULL;
        int i = 0;

        param = tokenizer(msg, "#;");

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


        uploadContainer->poids = atoi(param[1]);

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
/*                         outputVehicule()                       */
/*----------------------------------------------------------------*/

    char * outputVehicule(char *msg, char * FILEPARC)
    {
        FILE *fp;

        char * ret = (char *)malloc(MAXSTRING);
        char **param = NULL;
        int i=0;
        char * message = (char *)malloc(500);

        param = tokenizer(msg, "#;");

        fp = fopen(FILEPARC, "r+b");
        fseek(fp, 0L, SEEK_END); 
  
    	// calculating the size of the file 
    	int res = ftell(fp)/sizeof(Container);

        Container listContainer[res];
        Container* container;
        container = malloc(sizeof(Container));

        rewind(fp);
        while(fread(container, sizeof(Container), 1, fp))
        {
            if(strcmp(container->typeRetour, param[1]) == 0)
            {
                if(strcmp(container->destination, param[3]) == 0)
                {
                    strcat(message, toString(container));
                    printf("%s\n", message);
                    i++;
                }
            }
        }

        if (send(hSocketServ, message, sizeof(Container), 0) == -1)
        {
            printf("Erreur dans l'envoi de la liste\n");
            ret = "false";
        }
        else
        {
            printf("Liste envoyee\n");
            ret = "true";
        }

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
            printf("%s - %s - %d - %s - %s - %s - %d - %s\n", 
                container->idContainer, 
                container->coordonnees, 
                container->etat,
                container->dateReservation,
                container->dateArrivee,
                container->destination,
                container->poids,
                container->typeRetour);
        }

        free(container);
        fclose(fp);
	}

/*----------------------------------------------------------------*/
/*                              toString()                        */
/*----------------------------------------------------------------*/

    char * toString(struct Container * container)
    {
        char * msg = malloc(sizeof(Container));

        sprintf(msg, "%s - %s - %d - %s - %s - %s - %d - %s#", 
                container->idContainer, 
                container->coordonnees, 
                container->etat,
                container->dateReservation,
                container->dateArrivee,
                container->destination,
                container->poids,
                container->typeRetour);

        return msg;
    }