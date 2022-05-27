#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>        /* for manipulating directories */
#include <arpa/inet.h>       /* inet_ntoa() */
#define MAXLINE 4096         /*max text line length*/
#define LISTENQ 8            /*maximum number of client connections */
#define DIRECTORY_PATH_LEN 8 /* that is length of "./files/" */
#define DIRECTORY_STORAGE "./files"
#define SB_EOF "#EOF#"
#define MSG_FILE_EXISTED "[ ERROR ] File name already existed on server!\n"
#define MSG_FILE_AVAILABLE "File name available! Good to go!\n"
#define MSG_FILE_TRANSFER_CRASH "[ ERROR ] File transfering is interrupted!\n"
#define MSG_FILE_TRANSFER_SUCCESS "File transferred successfully!\n"

int handle_receive_file(int socket_fd, char *filename);
int is_available_file_name(char *filename);
char *extract_file_name_from_path(char *path);

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
        perror("[ ERROR ] Socket construction failed!\n");
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
        perror("[ ERROR ] Request listening failed!\n");
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
            printf("[ ERROR ] Connection not accepted!\n");
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
                char res[MAXLINE];
                int flag = 0;

                // Step 1: RECEIVE FILENAME from client
                len_recv = recv(connection_fd, buffer, MAXLINE, 0);
                buffer[len_recv] = '\0';
                if (len_recv == 0)
                {
                    printf("[ %s:%d ] Connection disconnected !\n",
                           inet_ntoa(addr_cli.sin_addr),
                           ntohs(addr_cli.sin_port));
                    exit(EXIT_SUCCESS);
                }

                printf("[ FROM CLIENT - %s:%d ]\n-->Received filename : %s\n",
                       inet_ntoa(addr_cli.sin_addr),
                       ntohs(addr_cli.sin_port), buffer);

                // Step 2: Check for file name duplicate
                char *filename = extract_file_name_from_path(buffer);
                if (!is_available_file_name(filename))
                {
                    perror(MSG_FILE_EXISTED);
                    strcpy(res, MSG_FILE_EXISTED);
                    flag = 1;
                }
                else
                {
                    printf(MSG_FILE_AVAILABLE);
                    strcpy(res, MSG_FILE_AVAILABLE);
                }
                // Step 3: Client response
                printf("\n[ TO CLIENT - %s:%d ]\n-->Sending data to client ...\n",
                       inet_ntoa(addr_cli.sin_addr),
                       ntohs(addr_cli.sin_port));

                len_send = send(connection_fd, res, strlen(res), 0);
                if (len_send < 0)
                {
                    perror("[ ERROR ] Message sent to client unsuccessfully!\n");
                }
                printf("-->Sent message : %s\n", res);

                // Step 4: Handle receiving file from client
                if (flag != 0)
                {
                    flag = 0;
                }
                else
                {
                    if (handle_receive_file(connection_fd, filename) != 0)
                    {
                        strcpy(res, MSG_FILE_TRANSFER_CRASH);
                    }
                    else
                    {
                        strcpy(res, MSG_FILE_TRANSFER_SUCCESS);
                    }
                    // send file transfering status message
                    len_send = send(connection_fd, res, strlen(res), 0);
                    if (len_send < 0)
                    {
                        perror("[ ERROR ] Message sent to client unsuccessfully!\n");
                    }
                    printf("-->Sent message : %s\n", res);
                }
                bzero(buffer, sizeof(buffer));
            }

            if (len_recv < 0)
            {
                perror("[ ERRROR ] Read error");
                exit(1);
            }
            close(connection_fd);
        }
    }
    // close listening socket
    close(connection_fd);
    return 0;
}

int handle_receive_file(int socket_fd, char *filename)
{
    int n;
    FILE *fptr;
    const int path_len = DIRECTORY_PATH_LEN + strlen(filename) + 1;
    char path[path_len];
    snprintf(path, sizeof(path), "%s/%s", DIRECTORY_STORAGE, filename);
    char buffer[MAXLINE];
    
    // Make storage directory if not existed
    struct stat st = {0};
    if (stat(DIRECTORY_STORAGE, &st) == -1)
    {
        mkdir(DIRECTORY_STORAGE, 0700);
    }
    fptr = fopen(path, "w");
    if (fptr == NULL)
    {
        printf("[ ERROR ] Cannot open file to write! Please retry\n");
        return 1;
    }
    while (1)
    {
        n = recv(socket_fd, buffer, MAXLINE, 0);
        if (n <= 0)
        {
            printf(MSG_FILE_TRANSFER_CRASH);
            return 1;
        }
        // EOF handling
        if (strcmp(buffer, SB_EOF) == 0)
        {
            break;
        }
        fprintf(fptr, "%s", buffer);
        bzero(buffer, sizeof(buffer));
    }
    fclose(fptr);
    printf("File stored successfully at %s !\n", path);
    return 0;
}

int is_available_file_name(char *filename)
{
    FILE *fptr;
    char path[DIRECTORY_PATH_LEN + strlen(filename) + 1];
    snprintf(path, sizeof(path), "./files/%s", filename);
    printf("Checking existence for file path %s ...\n", path);
    if ((fptr = fopen(path, "r")) != NULL)
    {
        fclose(fptr);
        return 0;
    }
    fclose(fptr);
    return 1;
}

// derive the file name
char *extract_file_name_from_path(char *path)
{
    const char delim[2] = "/";
    char *curToken = strtok(path, delim);
    char *prevToken = path;
    while (curToken != NULL)
    {
        prevToken = curToken;
        curToken = strtok(NULL, delim);
    }
    return prevToken;
}