#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAXLINE 4096 /*max text line length*/

int main(int argc, char **argv)
{
    int socket_fd;
    struct sockaddr_in addr_serv;
    char line_send[MAXLINE], line_recv[MAXLINE];

    // syntax checking
    if (argc != 3)
    {
        printf("Syntax error!\n-->Usage : ./client [SERVER_IP_ADDRESS] [PORT_NUMBER] (i.e. ./client 127.0.0.1 8888) !\n");
        exit(EXIT_FAILURE);
    }
    int const PORT = atoi(argv[2]);
    // validate port number input
    if (PORT == 0)
    {
        perror("Invalid port number specified!");
        exit(EXIT_FAILURE);
    }

    // Create a socket for the client
    // If sockfd < 0 there was an error in the creation of the socket
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Problem constructing the socket!\n");
        exit(2);
    }

    // Creation of the socket
    memset(&addr_serv, 0, sizeof(addr_serv));
    addr_serv.sin_family = AF_INET;
    addr_serv.sin_addr.s_addr = inet_addr(argv[1]);
    addr_serv.sin_port = htons(PORT); // convert to big-endian order

    // Connection of the client to the socket
    if (connect(socket_fd, (struct sockaddr *)&addr_serv, sizeof(addr_serv)) < 0)
    {
        perror("Problem connecting to the server!\n");
        exit(3);
    }

    printf("[ %s:%d ] Socket connected!\n",
           inet_ntoa(addr_serv.sin_addr),
           ntohs(addr_serv.sin_port));
    printf("[ TO SERVER ]\n-->Compile data to send (Enter for socket termination): ");

    while (fgets(line_send, MAXLINE, stdin) != NULL)
    {
        // sending data to server
        line_send[strlen(line_send)] = '\0';
        printf("[ TO SERVER ]\n-->Sent data : %s\n", line_send);
        int len_send = send(socket_fd, line_send, strlen(line_send), 0);
        if (line_send[0] == '\n')
        {
            printf("No data received. Socket about to be terminated...\n");
            close(socket_fd);
            break;
        }
        if (len_send < 0)
        {
            perror("Error sending data to server!\n");
            break;
        }

        // receiving data from server
        int len_recv = recv(socket_fd, line_recv, MAXLINE, 0);
        if (len_recv == 0)
        {
            // error: server terminated prematurely
            perror("The server terminated prematurely!\n");
            exit(4);
        }
        else if (len_recv < 0)
        {
            perror("Error retrieving data from server!\n");
            exit(5);
        }
        // happy case : data fetched successfully
        line_recv[len_recv] = '\0';
        printf("[ FROM SERVER ]\n-->Received data : %s\n", line_recv);
        printf("\n[ TO SERVER ]\n-->Compile data to send (Enter for socket termination) : ");
    }

    exit(EXIT_SUCCESS);
}