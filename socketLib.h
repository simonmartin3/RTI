//---------------------------------
// socketLib.h
//---------------------------------
// MARTIN Simon
//---------------------------------

#ifndef SOCKET_H
#define SOCKET_H

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

int SocketInit(struct sockaddr_in* , char* , unsigned long );
int SocketInitWithIpAddr(struct sockaddr_in*, char*, unsigned long, char* );
void ListenAccept(int *, int *, struct sockaddr *);

#endif
