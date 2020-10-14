#ifndef utilLib
#define utilLib

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <errno.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <time.h>
#include <pthread.h>

typedef struct Container Container;
struct Container
{
    char idContainer[20];
    char coordonnees[10];
    int  etat;
    char dateReservation[20];
    char dateArrivee[20];
    char destination[50];
    int  poids;
    char typeRetour[10];

};

typedef struct Vehicule Vehicule;
struct Vehicule
{
    char idVehicule[20];
    char destination[50];
    int  capacite;
};

int fctFile(char *nomFile);
void createLogin(char * FILELOG);
void createFichParc(char * FILEPARC);
char * authentification(char *msg, char * FILELOG);
char * createContainer(char *msg, char * FILEPARC);
char * container(char *msg, char * FILEPARC);
char * outputVehicule(char *msg, char * FILEPARC);
void displayContainer(char * FILEPARC);
char * toString(struct Container * container);
char ** tokenizer(char *msg, char *ct);
void pressEnter();
char * searchConfig(char *config);

#endif