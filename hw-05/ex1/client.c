#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 5500
#define BUFF_SIZE 1024

int main()
{
    int client_sock;
    char buff[BUFF_SIZE + 1];
    struct sockaddr_in server_addr; /* server's address information */
    int msg_len, bytes_sent, bytes_received, sent_bytes_count = 0;

    // Step 1: Construct socket
    client_sock = socket(AF_INET, SOCK_STREAM, 0);

    // Step 2: Specify server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);

    // Step 3: Request to connect server
    if (connect(client_sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) < 0)
    {
        printf("\n[ ERROR ] Can not connect to sever! Client exited prematurely!\n");
        return 0;
    }

    // Step 4: Communicate with server

    while (1)
    {
        // send message
        printf("\n[TO SERVER]\n-->Enter a string to send to server: ");
        memset(buff, '\0', (strlen(buff) + 1));
        fgets(buff, BUFF_SIZE, stdin);
        msg_len = strlen(buff);

        bytes_sent = send(client_sock, buff, msg_len, 0);
        if (bytes_sent < 0)
        {
            printf("[ ERROR ] Failed to send data!\n");
        }
        sent_bytes_count += bytes_sent;

        // receive echo reply
        bytes_received = recv(client_sock, buff, BUFF_SIZE, 0);
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
            break;
        }

        buff[bytes_received] = '\0';
        printf("[FROM SERVER]\n--> Response : %s", buff);
    }

    printf("Total bytes sent to server: %d\n", sent_bytes_count);

    // Step 4: Close socket
    close(client_sock);
    return 0;
}
