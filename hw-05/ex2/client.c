#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 5501
#define BUFF_SIZE 4096
#define MAX_LINE 4096

void send_file(FILE *fp, int sockfd);
void writefile(int sockfd, FILE *fp, ssize_t n);
ssize_t total = 0;

int main()
{
    int client_sock;
    struct sockaddr_in server_addr; /* server's address information */
    int msg_len, bytes_sent, bytes_received;
    char *filename, sendline[MAX_LINE];
    FILE *fp;
    char buff[BUFF_SIZE] = {0};
    char temp[BUFF_SIZE], filename2[BUFF_SIZE];

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
    printf("\n[TO SERVER]\n-->Enter a absolute path to send to server: ");
    fgets(sendline, MAX_LINE, stdin);

    if (sendline[0] == '\n')
    {
        exit(0);
    }

    sendline[strlen(sendline) - 1] = '\0';

    filename = basename(sendline);
    strcpy(temp, filename);
    if (filename == NULL)
    {
        perror("[ ERROR ]Can't get filename!\n");
        exit(1);
    }

    strncpy(buff, filename, strlen(filename));

    // send each line in file
    fp = fopen(sendline, "rb");
    if (fp == NULL)
    {
        perror("[ ERROR ]File not found!\n");
    }
    else
    {
        bytes_sent = send(client_sock, buff, MAX_LINE, 0);
        if (bytes_sent < 0)
        {
            printf("[ ERROR ] Failed to send data!\n");
        }
    }
    send_file(fp, client_sock);
    printf("[ INFO ] Sent successfully! NumBytes = %ld\n", total);

    strtok(temp, ".");
    strcpy(filename2, temp);
    strcat(filename2, "Capitalized.txt");
    fp = fopen(filename2, "wb");

    writefile(client_sock, fp, total);

    printf("Receive Success, NumBytes = %ld\n", total);
    printf("Successfully receive file from Server and rename to\"%s\"\n", filename2);

    fclose(fp);

    // Step 4: Close socket
    close(client_sock);
    return 0;
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
            perror("[ ERROR ] Reading file failed!\n");
            exit(1);
        }

        memset(sendlines, 0, MAX_LINE);
    }

    rewind(fp);
    sprintf(sendlines, "%ld", total);

    if (send(sockfd, sendlines, MAX_LINE, 0) == -1)
    {
        perror("[ ERROR ] Can't send file!\n");
        exit(1);
    }
    memset(sendlines, 0, MAX_LINE);

    while ((n = fread(sendlines, sizeof(char), MAX_LINE, fp)) > 0)
    {
        if (n != MAX_LINE && ferror(fp))
        {
            perror("[ ERROR ] Read File Error!\n");
            exit(1);
        }

        if (send(sockfd, sendlines, n, 0) == -1)
        {
            perror("[ ERROR ] Can't send file!\n");
            exit(1);
        }

        memset(sendlines, 0, MAX_LINE);
    }
}

void writefile(int sockfd, FILE *fp, ssize_t n)
{
    char buff[MAX_LINE] = {0}, res[MAX_LINE];
    int nLeft = n;

    while (nLeft > 0)
    {
        n = recv(sockfd, buff, MAX_LINE, 0);
        if (fwrite(buff, sizeof(char), n, fp) != n)
        {
            perror("[ ERROR ] Writing file failed!\n");
            exit(1);
        }
        nLeft -= n;
        memset(buff, 0, MAX_LINE);
    }
}
