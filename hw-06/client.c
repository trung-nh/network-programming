#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

#define PORT 5501
#define SERVER_ADDR "127.0.0.1"
#define BUFF_SIZE 1024

int main(int argc, const char *argv[])
{
    // create a socket
    int network_socket;
    network_socket = socket(AF_INET, SOCK_STREAM, 0);

    // specify an address for the socket
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = inet_addr(SERVER_ADDR);

    int connection_status = connect(network_socket, (struct sockaddr *)&server_address, sizeof(server_address));
    // check for connection_status
    if (connection_status == -1)
    {
        printf("\n[ ERROR ] Can not connect to sever! Client exited prematurely!\n");
        return 0;
    }
    printf("Successfully connected to server!\n");
    if (connection_status == 0)
    {
        char buff[BUFF_SIZE + 1];
        int total_bytes = 0;
        // receive data from the server
        while (1)
        {
            // recv(network_socket, &buff, sizeof(buff), 0);
            printf("\n[TO SERVER]\n-->Enter a string to send to server: ");
            memset(buff, '\0', (strlen(buff) + 1));
            fgets(buff, BUFF_SIZE, stdin);
            int send_status = send(network_socket, buff, sizeof(buff), 0);
            total_bytes += send_status;
            printf("Bytes sent: %d\n", total_bytes);
            int bytes_received = recv(network_socket, buff, sizeof(buff), 0);
            if (bytes_received < 0)
            {
                printf("[ ERROR ] Failed to receive data!\n");
            }
            else if (bytes_received == 0)
            {
                printf("[ INFO ] Connection terminated !\n");
                break;
            }
            if (strcmp(buff, "Q\n") == 0 || strcmp(buff, "q\n") == 0)
            {
                printf("Total bytes sent : %d\nYou are exiting program...\n", total_bytes);
                break;
            }
            // print out the server's response
            buff[bytes_received] = '\0';
            printf("[FROM SERVER]\n--> Response : %s", buff);
        }
    }
    // close the socket
    close(network_socket);

    return 0;
}
