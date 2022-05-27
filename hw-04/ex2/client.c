#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAXLINE 4096 /*max text line length*/
#define SB_EOF "#EOF#"
#define MAX_FILE_SIZE 100000000
#define MSG_FILE_EXISTED "[ ERROR ] File name already existed on server!\n"
#define MSG_FILE_TRANSFER_CRASH "[ ERROR ] File transfering is interrupted!\n"

void handle_send_file(FILE *fptr, int socket_fd);
void print_prompt();

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
    print_prompt();

    while (fgets(line_send, MAXLINE, stdin) != NULL)
    {
        line_send[strlen(line_send)] = '\0';
        if (line_send[0] == '\n')
        {
            printf("No data received. Socket about to be terminated...\n");
            close(socket_fd);
            exit(EXIT_SUCCESS);
        }

        // handle path and file name
        char path[strlen(line_send)];
        snprintf(path, sizeof(path), "%s", line_send);

        // Step 0: File validation
        // Step 0.0: Checking file existence on local machine
        FILE *fptr = fopen(path, "r");
        if (fptr == NULL)
        {
            perror("[ ERROR ] File not found!\n");
            // exit(EXIT_FAILURE);
            print_prompt();
            continue;
        }
        // Step 0.1: Checking file size (no more than 100MB)
        fseek(fptr, 0, SEEK_END);
        long int file_size = ftell(fptr);
        fseek(fptr, 0, SEEK_SET);
        printf("[ INFO ] File size : %ld (bytes)\n", file_size);
        if (file_size > MAX_FILE_SIZE)
        {
            printf("[ ERROR ] File size exceeded 100MB! File transfer terminated!\n\n");
            print_prompt();
            continue;
        }

        // Step 1: SEND FILE PATH to server for duplicate name checking
        int len_send = send(socket_fd, path, strlen(path), 0);
        printf("[ TO SERVER ]\n-->Sent file path : %s\n", path);

        if (len_send < 0)
        {
            perror("[ ERROR ] Data (filename) sent unsuccessfully!\n");
            // close(socket_fd);
            // exit(EXIT_FAILURE);
            print_prompt();
            continue;
        }

        // Step 2: RECEIVE RESPONSE from server
        int len_recv = recv(socket_fd, line_recv, MAXLINE, 0);
        if (len_recv == 0)
        {
            // error: server terminated prematurely
            perror("[ ERROR ] The server terminated prematurely!\n");
            close(socket_fd);
            exit(EXIT_FAILURE);
        }
        else if (len_recv < 0)
        {
            perror("[ ERROR ] Data retrieved from server unsuccessfully!\n");
            close(socket_fd);
            exit(EXIT_FAILURE);
        }
        // display received message
        line_recv[len_recv] = '\0';
        printf("[ FROM SERVER ]\n-->Received message : %s\n", line_recv);

        // Step 3: SEND FILE
        if (strcmp(line_recv, MSG_FILE_EXISTED) != 0)
        {
            handle_send_file(fptr, socket_fd);
            printf("[ SUCCESS ] File data sent successfully!\n");

            // receive file transfering status message
            len_recv = recv(socket_fd, line_recv, MAXLINE, 0);
            line_recv[len_recv] = '\0';
            printf("[ FROM SERVER ]\n-->Received message : %s\n", line_recv);
        }

        // continue loop iteration for next file transfer
        print_prompt();
    }

    // socket disconnecting
    printf("[ %s:%d ] Socket disconnected!\n",
           inet_ntoa(addr_serv.sin_addr),
           ntohs(addr_serv.sin_port));
    close(socket_fd);
    exit(EXIT_SUCCESS);
}

void print_prompt()
{
    printf("[ TO SERVER ]\n-->Enter file path to send (Enter for socket termination): ");
}
void handle_send_file(FILE *fptr, int socket_fd)
{
    char buffer[MAXLINE] = {0};

    while (fgets(buffer, MAXLINE, fptr) != NULL)
    {
        if (send(socket_fd, buffer, sizeof(buffer), 0) == -1)
        {
            perror("[ ERROR ] File sending failed!\n.");
            exit(1);
        }
        bzero(buffer, sizeof(buffer));
    }
    // send EOF signal to server
    send(socket_fd, SB_EOF, sizeof(SB_EOF), 0);
}