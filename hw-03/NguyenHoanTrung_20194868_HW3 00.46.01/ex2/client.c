#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAXLINE 250

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        printf("ERROR: Please include the server IP address and port number\n");
        exit(EXIT_FAILURE);
    }
    if (argc > 3)
    {
        printf("ERROR: Too many arguments in program call\n");
        exit(EXIT_FAILURE);
    }

    int sockfd, n, from_len;
    struct sockaddr_in servaddr, from_socket;
    socklen_t addrlen = sizeof(from_socket);
    char sendline[MAXLINE], recvline[MAXLINE + 1];

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(argv[2]));
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    printf("Socket created !\n");
    printf("[ TO SERVER ]\n-->To-be sent data: ");

    while (fgets(sendline, MAXLINE, stdin) != NULL)
    {
        if (sendline[0] == '\n')
            break;
        sendline[strlen(sendline)] = '\0';
        printf("To Server: %s\n", sendline);
        sendto(sockfd, sendline, strlen(sendline), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));

        n = recvfrom(sockfd, recvline, MAXLINE, 0, (struct sockaddr *)&from_socket, &addrlen);
        recvline[strlen(recvline)] = '\0'; // null terminate
        printf("[ FROM SERVER ]\n-->Received dataa : %s\n", recvline);

        for (int i = strlen(sendline) - 1; i >= 0; i--)
        {
            sendline[i] = '\0';
        }

        for (int i = strlen(recvline) - 1; i >= 0; i--)
        {
            recvline[i] = '\0';
        }
        printf("\n[ TO SERVER ]\n-->To-be sent data: ");
    }

    close(sockfd);
}
