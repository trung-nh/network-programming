#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/wait.h>
#include <errno.h>
#include <arpa/inet.h>
#include <ctype.h>

#define PORT 5501
#define BACKLOG 20
#define BUFF_SIZE 4096
#define MAX_LINE 4096

ssize_t total = 0;

/* Handler process signal*/
void sig_chld(int signo);

/*
 * Receive and echo message to client
 * [IN] sockfd: socket descriptor that connects to client
 */
void echo(int sockfd);

void writefile(int sockfd, FILE *fp);
void send_file(FILE *fp, int sockfd);
char *stringProcess(char *buff);

int main()
{

    int listen_sock, conn_sock; /* file descriptors */
    struct sockaddr_in server;  /* server's address information */
    struct sockaddr_in client;  /* client's address information */
    pid_t pid;
    socklen_t sin_size;

    if ((listen_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    { /* calls socket() */
        perror("\n[ ERROR ] Socket construction failed!\n");
        return 0;
    }

    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = htonl(INADDR_ANY); /* INADDR_ANY puts your IP address automatically */

    printf("[ INFO ] Socket constructed!\n");
    if (bind(listen_sock, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        perror("\n[ ERROR ] Binding failed!\n");
        return 0;
    }
    printf("[ INFO ] Socket bound!\n");
    if (listen(listen_sock, BACKLOG) == -1)
    {
        perror("\n[ ERROR ] Listening failed!\n");
        return 0;
    }
    printf("[ INFO ] Listening on port %d...\n", PORT);
    /* Establish a signal handler to catch SIGCHLD */
    signal(SIGCHLD, sig_chld);

    while (1)
    {
        sin_size = sizeof(struct sockaddr_in);
        if ((conn_sock = accept(listen_sock, (struct sockaddr *)&client, &sin_size)) == -1)
        {
            if (errno == EINTR)
            {
                continue;
            }
            else
            {
                printf("[ ERROR ] Connection refused!\n");
                return 0;
            }
        }

        /* For each client, fork spawns a child, and the child handles the new client */
        pid = fork();

        /* fork() is called in child process */
        if (pid == 0)
        {
            close(listen_sock);
            printf("[ INFO ] New connection from IP %s\n", inet_ntoa(client.sin_addr)); /* prints client's IP */
            echo(conn_sock);
            exit(0);
        }

        /* The parent closes the connected socket since the child handles the new client */
        close(conn_sock);
    }
    close(listen_sock);
    return 0;
}

void sig_chld(int signo)
{
    pid_t pid;
    int stat;

    /* Wait the child process terminate */
    while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)
    {
        printf("\nChild %d terminated\n", pid);
    }
}

void echo(int sockfd)
{
    char buff[BUFF_SIZE];
    int bytes_sent, bytes_received;
    char addr[INET_ADDRSTRLEN];
    char filename[BUFF_SIZE] = {0}, filename2[BUFF_SIZE], *temp;
    FILE *fp;

    recv(sockfd, filename, BUFF_SIZE, 0);

    temp = strtok(filename, ".");
    strcpy(filename2, temp);
    strcat(filename2, "CapitalizedByServer.txt");

    fp = fopen(filename2, "wb");
    writefile(sockfd, fp);
    printf("Receive Success, NumBytes = %ld\n", total);
    fclose(fp);

    fp = fopen(filename2, "r");
    send_file(fp, sockfd);
    printf("Successfully send file \"%s\" to client\n", filename2);
    fclose(fp);

    close(sockfd);
}

void writefile(int sockfd, FILE *fp)
{
    ssize_t n;
    char buff[MAX_LINE] = {0}, res[MAX_LINE];
    recv(sockfd, buff, MAX_LINE, 0);
    total = atoi(buff);
    int nLeft = total;

    while (nLeft > 0)
    {
        n = recv(sockfd, buff, MAX_LINE, 0);
        strcpy(res, stringProcess(buff));

        if (n == -1)
        {
            printf("Error: File transferring is interrupted!\n");
            break;
        }

        if (fwrite(res, sizeof(char), n, fp) != n)
        {
            perror("Write File Error");
            exit(1);
        }
        nLeft -= n;
        memset(buff, 0, MAX_LINE);
    }
}

char *stringProcess(char *buff)
{
    int length = strlen(buff);
    for (int i = 0; i < length; i++)
    {
        buff[i] = toupper(buff[i]);
    }
    return buff;
}

void send_file(FILE *fp, int sockfd)
{
    int n;
    char sendlines[MAX_LINE] = {0};
    total = 0;

    while ((n = fread(sendlines, sizeof(char), MAX_LINE, fp)) > 0)
    {
        total += n;
        if (n != MAX_LINE && ferror(fp))
        {
            perror("Read File Error!\n");
            exit(1);
        }

        if (send(sockfd, sendlines, n, 0) == -1)
        {
            perror("Can't send file!\n");
            exit(1);
        }

        memset(sendlines, 0, MAX_LINE);
    }
}
