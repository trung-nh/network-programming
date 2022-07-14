#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#define PORT 5501
#define LISTENQ 3
#define THREAD_TOTAL 3
#define SERV_MSG_LEN 100
#define CLI_MSG_LEN 255

// Remember to use -pthread when compiling this server's source code
void *connection_handler(void *);

int main()
{
	// char server_message[SERV_MSG_LEN] = "Hello from Server!!\n";
	int server_socket;
	struct sockaddr_in addr_serv;
	server_socket = socket(AF_INET, SOCK_STREAM, 0);

	// socket construction
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket < 0)
	{
		perror("Error constructing socket!\n");
		exit(EXIT_FAILURE);
	}
	printf("Server socket successfully constructed!\n");

	// socket binding
	bzero(&addr_serv, sizeof(addr_serv));
	addr_serv.sin_family = AF_INET;
	addr_serv.sin_addr.s_addr = htonl(INADDR_ANY);
	addr_serv.sin_port = htons(PORT);

	// bind the socket to the specified IP addr and port
	if (bind(server_socket, (struct sockaddr *)&addr_serv, sizeof(addr_serv)) != 0)
	{
		perror("Socket binding failed!\n");
		exit(EXIT_FAILURE);
	}
	printf("Server socket successfully bound!\n");

	// request listening
	if (listen(server_socket, LISTENQ))
	{
		perror("Request listening failed!\n");
		exit(EXIT_FAILURE);
	}

	printf("Server running at port %d ...waiting for connections.\n", PORT);

	int no_threads = 0;
	pthread_t threads[THREAD_TOTAL];

	while (no_threads < THREAD_TOTAL)
	{
		printf("Listening...\n");
		int client_socket = accept(server_socket, NULL, NULL);
		puts("Connection accepted");
		if (pthread_create(&threads[no_threads], NULL, connection_handler, &client_socket) < 0)
		{
			perror("[ERROR] Could not create thread!");
			return 1;
		}
		if (client_socket < 0)
		{
			printf("Connection not accepted!\n");
			// exit(EXIT_FAILURE);
			break;
		}
		printf("New connection accepted!\n");
		puts("Handler assigned");
		no_threads++;
	}
	int k = 0;
	for (k = 0; k < 3; k++)
	{
		pthread_join(threads[k], NULL);
	}

	// int send_status;
	// send_status=send(client_socket, server_message, sizeof(server_message), 0);
	close(server_socket);

	return 0;
}

void *connection_handler(void *client_socket)
{
	int socket = *(int *)client_socket;
	int bytes_received, bytes_sent;
	// char server_message[SERV_MSG_LEN] = "Successfully connected to server!\n";
	// bytes_sent = send(socket, server_message, sizeof(server_message), 0);
	char client_message[CLI_MSG_LEN];
	while ((bytes_received = recv(socket, client_message, 100, 0)) > 0)
	{
		// end of string marker
		client_message[bytes_received] = '\0';
		if (strcmp(client_message, "q") == 0 || strcmp(client_message, "Q") == 0)
		{
			printf("A connection disconnected!\n");
			break;
		}
		// Send the message back to client
		for (int i = 0; i < bytes_received; i++)
		{
			if (client_message[i] >= 'a' && client_message[i] <= 'z')
			{
				client_message[i] = client_message[i] - 32;
			}
		}
		bytes_sent = send(socket, client_message, bytes_received, 0);
		if (bytes_sent < 0)
		{
			printf("[ ERROR ] Failed to respond to client!\n");
		}
	}

	return 0;
}
