#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <ctype.h>
#define MAXLINE 255

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Please provide a port number to run server at !\n");
        exit(EXIT_FAILURE);
        // return 1;
    }
    int PORT = atoi(argv[1]);
    int sockfd, received_msg_len;
    socklen_t socket_len;
    char message[MAXLINE];
    struct sockaddr_in servaddr, cliaddr;

    // Initiate a socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    // Bind address to the newly created socket
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);
    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) >= 0)
    {
        printf("Server is running at port %d", PORT);
    }
    else
    {
        perror("Socket binding failed !");
        exit(EXIT_FAILURE);
        // return 1;
    }

    // Interact with connected client(s)
    for (;;)
    {
        socket_len = sizeof(cliaddr);

        printf("\n[ FETCHING DATA FROM CLIENT ] ...");

        received_msg_len = recvfrom(sockfd, message, MAXLINE, 0, (struct sockaddr *)&cliaddr, &socket_len);
        printf("\n[ FROM CLIENT ]\n-->Received data: %s", message);

        char res_digits[MAXLINE], res_alpha[MAXLINE], res[MAXLINE];
        int id_digits = 0, id_alpha = 0, is_invalid = 0, i;        

        printf("\n[ TO CLIENT ]\n-->Sending data to client ...\n");
        
        // debug
        // printf("msg=%s\tlen=%d\n", message, received_msg_len);

        for (i = 0; i < received_msg_len - 1; i++)
        {
            char c = message[i];
            int d = c - '0';            
            if (c >= '0' && c <= '9')
            {
                res_digits[id_digits++] = c;
            }
            else if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
            {
                res_alpha[id_alpha++] = c;
            }
            else
            {
                printf("Syntax error ! Message malformed !\n");
                strcpy(res, "Error");
                is_invalid = 1;
            }
        }
        if (is_invalid == 0)
        {
            res_alpha[id_alpha] = '\0';
            res_digits[id_digits] = '\0';
            printf("%s\n", res_digits);
            printf("%s\n", res_alpha);
            strcpy(res, res_digits);
            strcat(res, " ");
            strcat(res, res_alpha);
        }

        // Send data to corresponding client
        sendto(sockfd, res, strlen(res), 0, (struct sockaddr *)&cliaddr, socket_len);

        strcpy(res, "");
        for (int i = strlen(message) - 1; i >= 0; i--)
        {
            message[i] = '\0';
        }
        is_invalid = 0;
    }

    // Terminate socket
    close(sockfd);
}