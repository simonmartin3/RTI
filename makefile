.SILENT :

mainS: 	serverContainers.o
		echo Compilation de mainS
		gcc -Wall -o mainS serverContainers.o -lnsl -lsocket -lpthread

mainA: 	appliContainers.o
		echo Compilation de mainA
		gcc -Wall -o mainA appliContainers.o -lnsl -lsocket -lpthread

serverContainers.o: serverContainers.c libraryServer.h
		echo Compilation de serverContainers
		gcc -Wall -c serverContainers.c

appliContainers.o: appliContainers.c libraryServer.h
		echo Compilation de appliContainers
		gcc -Wall -c appliContainers.c

clean : 
	rm -rf *.o mainA mainS


