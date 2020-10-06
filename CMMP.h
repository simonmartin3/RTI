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

char * login();
char * logout();
/*void inputTruck(int*, int*);
void inputDone(char*, int*);
void outputReady(int*, char*, int*);
void outputOne(int*);
void outputDone(int*, int*);
void logout(char*, char*);*/

#endif
