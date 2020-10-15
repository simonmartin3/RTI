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

	void createFich(char * nameFile)
	{
		FILE *fichFile;

		fichFile = fopen(nameFile, "wb");
		
		fclose(fichFile);
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

        //free(newContainer);
        fclose(fp);

        return ret;
    }

/*----------------------------------------------------------------*/
/*                        uploadContainer()                       */
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
/*                        moveContainer()                       */
/*----------------------------------------------------------------*/

    char * moveContainer(char *msg, char * FILEPARC, char * FILEVEHICULE)
    {
        FILE *fp, *fp2;

        char * ret = (char *)malloc(MAXSTRING);
        int i = 0, j = 0, findC = 0, findV = 0;

        fp = fopen(FILEPARC, "r+b");
        fp2 = fopen(FILEVEHICULE, "r+b");

        Container* uploadContainer;
        uploadContainer = malloc(sizeof(Container));
        Vehicule *uploadVehicule;
        uploadVehicule = malloc(sizeof(Vehicule));

        while(fread(uploadContainer, sizeof(Container), 1, fp))
        {
            if(strcmp(uploadContainer->idContainer, msg) == 0)
            {
                printf("Trouve\n");
                findC = 1;
                break;
            }
            i++;
            findC = 0;
        }

        if(findC == 1)
        {

            while(fread(uploadVehicule, sizeof(Vehicule), 1, fp2))
            {
                if(strcmp(uploadVehicule->typeVehicule, uploadContainer->typeRetour) == 0)
                {
                    if(strcmp(uploadVehicule->destination, uploadContainer->destination) == 0)
                    {
                        
                        printf("Trouve\n");
                        findV = 1;
                        break;
                    }
                }
                j++;
                findV = 0;
            }

            if(findV == 1)
            {
                if(uploadVehicule->capacite != 0)
                {
                    uploadVehicule->capacite = uploadVehicule->capacite-1;

                    fseek(fp2, j*sizeof(Vehicule), SEEK_SET);

                    if(fwrite(uploadVehicule, sizeof(Vehicule), 1, fp2) != 0)  
                    {
                        printf("Le vehicule a bien ete modifie !\n");
                        ret = OK;
                    }
                    else 
                    {    
                        printf("Erreur d'ecriture dans le fichier !\n"); 
                        ret = FAIL;
                    }

                    uploadContainer->etat = 0;

                    fseek(fp, i*sizeof(Container), SEEK_SET);

                    if(fwrite(uploadContainer, sizeof(Container), 1, fp) != 0)  
                    {
                        printf("Le container a bien ete chargé !\n");
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
                    strcpy(ret, uploadVehicule->idVehicule);
                }
            }
            else
            {
                ret = FAIL;
            }
        }
        else
        {
            ret = FAIL;
        }

        free(uploadContainer);
        free(uploadVehicule);
        fclose(fp);
        fclose(fp2);

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
/*                      displayVehicule()                        */
/*----------------------------------------------------------------*/

    char * displayVehicule(char * FILEVEHICULE)
    {
        FILE *fp;

        fp = fopen(FILEVEHICULE, "rb");

        Vehicule* vehicule;
        vehicule = malloc(sizeof(Vehicule));
        
        printf("Afficher vehicule\n");

        while(fread(vehicule, sizeof(Vehicule), 1, fp))
        {
            printf("%s - %s - %d\n", 
                vehicule->idVehicule, 
                vehicule->destination,
                vehicule->capacite);
        }

        free(vehicule);
        fclose(fp);

        return OK;
    }

/*----------------------------------------------------------------*/
/*                              toString()                        */
/*----------------------------------------------------------------*/

    char * toString(struct Container * container)
    {
        char * msg = malloc(sizeof(Container));

        sprintf(msg, "%s - %s - %d - %s - %s - %s - %d - %s", 
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