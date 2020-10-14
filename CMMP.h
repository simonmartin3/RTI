/*----------------------------------------*/
/*					CMMP.h				  */
/*----------------------------------------*/
/*				MARTIN Simon		      */
/*----------------------------------------*/

#ifndef CMMP
#define CMMP

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

#define MAXSTRING	100

typedef struct Message Message;
struct Message
{
    int typeReq;
    char msg[MAXSTRING];
};

struct Message login();
struct Message logout();
struct Message inputTruck();
struct Message inputDone(char * idContainer);
struct Message outputReady();
struct Message outputOne(char * idContainer);
// void outputDone(int*, int*);
// void logout(char*, char*);

#endif
