.SILENT :

RTI:	socketLib.o Serveur Client

Serveur: 	serverContainers.c socketLib.o
			echo Compilation de serveur...
			gcc -Wall -o Ser serverContainers.c socketLib.o -lnsl -lsocket

Client: 	appliContainers.c socketLib.o
			echo Compilation de client...
			gcc -Wall -o Cli appliContainers.c socketLib.o -lnsl -lsocket

socketLib.o: 	socketLib.c
				echo Compilation de la librairie...
				gcc -Wall -c socketLib.c -lnsl -lsocket -lpthread -D SUN -m64

# appliContainers.o: 	appliContainers.c libraryServer.h
					# echo Compilation de appliContainers
					# gcc -Wall -c appliContainers.c -lnsl -lsocket -lpthread -D SUN -m64

clean : 
	rm -rf *.o

kill:
	kill -9 *


