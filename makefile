.SILENT :

main: 	serverContainers.o
		echo Compilation de main
		gcc -Wall -o main serverContainers.o -lnsl -lsocket -lpthread

serverContainers.o: serverContainers.c
		echo Compilation de server_containers
		gcc -Wall -c serverContainers.c


