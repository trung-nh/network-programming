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
        printf("Server IP address and port number required !\n");
        exit(EXIT_FAILURE);
    }
    if (argc > 3)
    {
        printf("Syntax error : Too many arguments !");
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
    printf("Created socket\n");

    while (fgets(sendline, MAXLINE, stdin) != NULL)
    {
        int sendline_len = strlen(sendline);
        if (sendline[0] == '\n')
        {
            printf("\nSocket connection terminated !\n");
            break;
        }
        sendline[sendline_len] = '\0';
        printf("[ TO SERVER ]\n-->Sent data: %s\n", sendline);
        int sent_msg_len = sendto(sockfd, sendline, sendline_len, 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
        printf("Message sent with length = %d\n",sent_msg_len);
        n = recvfrom(sockfd, recvline, MAXLINE, 0, (struct sockaddr *)&from_socket, &addrlen);
        recvline[strlen(recvline)] = '\0'; // null terminate
        printf("[ FROM SERVER ]\n-->IP address: %s\n-->Port number: %d\n-->Received data: %s\n", inet_ntoa(from_socket.sin_addr), htons(from_socket.sin_port), recvline);
        for (int i = sendline_len - 1; i >= 0; i--)
        {
            sendline[i] = '\0';
        }

        for (int i = strlen(recvline) - 1; i >= 0; i--)
        {
            recvline[i] = '\0';
        }
    }

    close(sockfd);
}
