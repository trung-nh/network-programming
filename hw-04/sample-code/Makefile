all: tcpServer tcpClient fileServer fileClient

tcpServer: tcpServer.o
	gcc tcpServer.o -o tcpServer
	   
tcpServer.o: tcpServer.c
	gcc -c tcpServer.c -o tcpServer.o
	
tcpClient: tcpClient.o
	gcc tcpClient.o -o tcpClient
	   
tcpClient.o: tcpClient.c
	gcc -c tcpClient.c -o tcpClient.o

fileClient: fileClient.o
	gcc fileClient.o -o fileClient
	   
fileClient.o: fileClient.c
	gcc -c fileClient.c -o fileClient.o

fileServer: fileServer.o
	gcc fileServer.o -o fileServer
	   
fileServer.o: fileServer.c
	gcc -c fileServer.c -o fileServer.o
	
clean: 	     
	rm -rf *o tcpServer tcpClient fileServer fileClient