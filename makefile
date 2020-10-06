.SILENT :

RTI:	socketLib.o CMMP.o Serveur Client

Serveur: 	serverContainers.c socketLib.o CMMP.o
			echo Compilation de serveur...
			gcc -Wall -o Ser serverContainers.c socketLib.o CMMP.o -lnsl -lsocket

Client: 	appliContainers.c socketLib.o CMMP.o 
			echo Compilation de client...
			gcc -Wall -o Cli appliContainers.c socketLib.o CMMP.o -lnsl -lsocket

socketLib.o: 	socketLib.c
				echo Compilation de la librairie...
				gcc -Wall -c socketLib.c -lnsl -lsocket -lpthread -D SUN -m64

CMMP.o: 	CMMP.c
				echo Compilation de la librairie CMMP...
				gcc -Wall -c CMMP.c -lnsl -lsocket -lpthread -D SUN -m64

# appliContainers.o: 	appliContainers.c libraryServer.h
					# echo Compilation de appliContainers
					# gcc -Wall -c appliContainers.c -lnsl -lsocket -lpthread -D SUN -m64

clean : 
	rm -rf *.o

kill:
	kill -9 *


