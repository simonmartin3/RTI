#include "utilLib.h"

#define MAXSTRING	100
#define SERVEURCONF "serveur.conf"
#define OK "true"
#define FAIL "false"

/*----------------------------------------------------------------*/
/*                            tokenizer()                         */
/*----------------------------------------------------------------*/
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

/*----------------------------------------------------------------*/
/*                            pressEnter()                        */
/*----------------------------------------------------------------*/

void pressEnter()
{
    printf("Press enter to continue...");
    getchar();
}

/*----------------------------------------------------------------*/
/*                           searchConfig()                       */
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

            param = tokenizer(msg, ";");

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


        param = tokenizer(msg, ";");

        Container* newContainer;
        newContainer = malloc(sizeof(Container));

        strcpy(newContainer->idContainer, param[1]);
        
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
            ret = newContainer->idContainer;
        }
        else 
        {    
            printf("Erreur d'ecriture dans le fichier !\n"); 
            ret = FAIL;
        }

        free(newContainer);
        fclose(fp);

        return ret;
    }

/*----------------------------------------------------------------*/
/*                      	 container()                          */
/*----------------------------------------------------------------*/

	char * uploadContainer(char *msg, char * FILEPARC)
	{
        FILE *fp;

        char * ret = (char *)malloc(MAXSTRING);
        int i = 0, find = 0;
        char **param = NULL;

        param = tokenizer(msg, ";");

        fp = fopen(FILEPARC, "r+b");

        Container* uploadContainer;
        uploadContainer = malloc(sizeof(Container));

        while(fread(uploadContainer, sizeof(Container), 1, fp))
        {
            if(strcmp(uploadContainer->idContainer, param[0]) == 0)
            {
                printf("Trouve\n");
                find = 1;
                break;
            }
            i++;
            find = 0;
        }

        if(find == 1)
        {
            uploadContainer->poids = atoi(param[1]);

            fseek(fp, i*sizeof(Container), SEEK_SET);

            if(fwrite(uploadContainer, sizeof(Container), 1, fp) != 0)  
            {
                printf("Le container a bien ete modifie !\n");
                ret = OK;
            }
            else 
            {    
                printf("Erreur d'ecriture dans le fichier !\n"); 
                ret = FAIL;
            }
        }
        else
        {
            ret = FAIL;
        }

        free(uploadContainer);
        fclose(fp);

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
        char * message = (char *)malloc(500);
        int i = 0;

        param = tokenizer(msg, ";");

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
            if(strcmp(container->typeRetour, param[0]) == 0)
            {
                if(strcmp(container->destination, param[2]) == 0)
                {
                    memcpy(listContainer[i], container, sizeof(Container));
                    i++;
                }
            }
        }

        for(int j = 0; j < i; j++)
        {
            printf("%s\n", &listContainer[j].idContainer);
        }

        ret = OK;
        return ret;
    }

/*----------------------------------------------------------------*/
/*                      displayContainer()                        */
/*----------------------------------------------------------------*/

	char * displayContainer(char * FILEPARC)
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

        return OK;
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