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

int fctFile(char *nomFile);
void createLogin(char * FILELOG);
void createFichParc(char * FILEPARC);
char * authentification(char *msg,, char * FILELOG);
char * createContainer(char *msg,, char * FILEPARC);
char * container(char *msg,, char * FILEPARC);
char * outputVehicule(char *msg,, char * FILEPARC);
void displayContainer();
char * toString(struct Container * container);

#endif