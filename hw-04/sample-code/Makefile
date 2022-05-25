all: tcpServer tcpClient

tcpServer: tcpServer.o
	gcc tcpServer.o -o tcpServer
	   
tcpServer.o: tcpServer.c
	gcc -c tcpServer.c -o tcpServer.o
	
tcpClient: tcpClient.o
	gcc tcpClient.o -o tcpClient
	   
tcpClient.o: tcpClient.c
	gcc -c tcpClient.c -o tcpClient.o
	
clean: 	     
	rm -rf *o tcpServer tcpClient