.SILENT :

run: 	serverContainers.o
		echo Compilation de run
		gcc -Wall -o run serverContainers.o -lnsl -lsocket

serverContainers.o: serverContainers.c
		echo Compilation de server_containers
		gcc -Wall -c serverContainers.c


