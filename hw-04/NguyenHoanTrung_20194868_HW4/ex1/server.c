#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h> /* inet_ntoa() */
#define MAXLINE 4096   /*max text line length*/
#define LISTENQ 8      /*maximum number of client connections */

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Syntax error!\n-->Usage : ./server [PORT_NUMBER] (i.e. ./server 8888) !\n");
        exit(EXIT_FAILURE);
    }

    int const PORT = atoi(argv[1]);
    // validate port number input
    if (PORT == 0)
    {
        perror("Invalid port number specified!");
        exit(EXIT_FAILURE);
    }

    int socket_fd, connection_fd, len_recv, len_send;
    socklen_t len_addr_cli;
    struct sockaddr_in addr_cli, addr_serv;
    char buffer[MAXLINE];
    pid_t childpid;

    // socket construction
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0)
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

    if (bind(socket_fd, (struct sockaddr *)&addr_serv, sizeof(addr_serv)))
    {
        perror("Socket binding failed!\n");
        exit(EXIT_FAILURE);
    }
    printf("Server socket successfully bound!\n");

    // request listening
    if (listen(socket_fd, LISTENQ))
    {
        perror("Request listening failed!\n");
        exit(EXIT_FAILURE);
    }

    printf("Server running at port %d ...waiting for connections.\n", PORT);

    // clients communication
    for (;;)
    {
        len_addr_cli = sizeof(addr_cli);
        connection_fd = accept(socket_fd, (struct sockaddr *)&addr_cli, &len_addr_cli);
        if (connection_fd < 0)
        {
            printf("Connection not accepted!\n");
            // exit(EXIT_FAILURE);
            break;
        }
        printf("[ %s:%d ] New connection accepted!\n",
               inet_ntoa(addr_cli.sin_addr),
               ntohs(addr_cli.sin_port));

        if ((childpid = fork()) == 0)
        {
            close(socket_fd);
            while (1)
            {
                char res_digits[MAXLINE], res_alpha[MAXLINE], res[MAXLINE];
                int id_digits = 0, id_alpha = 0, is_invalid = 0, i;
                len_recv = recv(connection_fd, buffer, MAXLINE, 0);
                buffer[len_recv] = '\0';                
                if (len_recv == 1)
                {
                    printf("[ %s:%d ] Connection disconnected !\n",
                           inet_ntoa(addr_cli.sin_addr),
                           ntohs(addr_cli.sin_port));
                    exit(EXIT_SUCCESS);
                }

                printf("[ FROM CLIENT - %s:%d ]\n-->Received data : %s\n",
                       inet_ntoa(addr_cli.sin_addr),
                       ntohs(addr_cli.sin_port), buffer);

                // client response
                printf("\n[ TO CLIENT - %s:%d ]\n-->Sending data to client ...\n",
                       inet_ntoa(addr_cli.sin_addr),
                       ntohs(addr_cli.sin_port));

                // string handling logics
                for (i = 0; i < len_recv - 1; i++)
                {
                    char c = buffer[i];
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
                        strcpy(res, "[ Error ]");
                        is_invalid = 1;
                    }
                }
                if (is_invalid == 0)
                {
                    res_alpha[id_alpha] = '\0';
                    res_digits[id_digits] = '\0';
                    strcpy(res, res_digits);
                    strcat(res, " ");
                    strcat(res, res_alpha);
                }

                // send data to client
                len_send = send(connection_fd, res, strlen(res), 0);
                if (len_send < 0)
                {
                    perror("Error sending data to client!\n");
                }
                printf("-->Sent data : %s\n", res);

                is_invalid = 0;
                bzero(buffer, sizeof(buffer));
            }

            if (len_recv < 0)
            {
                perror("Read error");
                exit(1);
            }
            close(connection_fd);
        }
    }
    // close listening socket
    close(connection_fd);
    return 0;
}
