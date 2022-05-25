#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#define MAXLINE 255
#define NOT_FOUND_INFORMATION 1
#define INVALID_ADDRESS 2

struct hostent *he;
struct in_addr **addr_list;
struct in_addr **name_list;
struct in_addr addr;
int flag = 0;
char res[MAXLINE];

int is_digit_string(char *str)
{
    if (str == NULL)
    {
        return 0;
    }
    while (*str)
    {
        if (!isdigit(*str))
        {
            return 0;
        }
        str++;
    }
    return 1;
}

int is_valid_ip(char *ip)
{
    int i, num, dots = 0;
    char *ptr;
    if (ip == NULL)
    {
        return 0;
    }
    ptr = strtok(ip, ".");
    if (ptr == NULL)
    {
        return 0;
    }
    while (ptr)
    {
        if (!is_digit_string(ptr))
        {
            return 0;
        }
        num = atoi(ptr);
        if (num < 0 || num > 255)
        {
            return 0;
        }
        ptr = strtok(NULL, ".");
        if (ptr != NULL)
        {
            dots++;
        }
    }
    if (dots != 3)
    {
        return 0;
    }
    return 1;
}

void resolve_domain_name_from_ip(char *ip)
{
    int i;
    inet_aton(ip, &addr);
    he = gethostbyaddr(&addr, sizeof(addr), AF_INET);
    if (he == NULL)
    {
        flag = NOT_FOUND_INFORMATION;
        return;
    }

    strcpy(res, "Official name: ");
    strcat(res, he->h_name);
    strcat(res, "\nAlias name:\n");
    if (he->h_aliases[0] != NULL)
    {
        for (i = 0; he->h_aliases[i] != NULL; i++)
        {
            strcat(res, he->h_aliases[i]);
        }
    }
    else
    {
        strcat(res, "This IP does not have alias name!\n");
    }
}

void resolve_ip_from_domain_name(char *name)
{
    int i;
    he = gethostbyname(name);
    if (he == NULL)
    {
        flag = NOT_FOUND_INFORMATION;
        return;
    }

    char *temp;
    strcpy(res, "Official name: ");
    temp = inet_ntoa(*(struct in_addr *)he->h_addr);
    strcat(res, temp);
    strcat(res, "\nAlias name:\n");
    addr_list = (struct in_addr **)he->h_addr_list;

    if (addr_list[0] != NULL)
    {
        for (i = 0; addr_list[i] != NULL; i++)
        {
            temp = inet_ntoa(*addr_list[i]);
            strcat(res, temp);
            strcat(res, "\n");
        }
    }
    else
    {
        strcat(res, "This IP does not have alias IP!\n");
    }
}

int main(int argc, char *argv[])
{
    int SERV_PORT = atoi(argv[1]), i;
    int sockfd, received_msg_len;
    socklen_t len;
    char message[MAXLINE];
    struct sockaddr_in servaddr, cliaddr;

    // Initiate a socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    // Bind address to the newly created socket
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);
    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) >= 0)
    {
        printf("Server is running at port %d", SERV_PORT);
    }
    else
    {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Interact with connected client(s)
    for (;;)
    {
        len = sizeof(cliaddr);
        printf("\n[ FETCHING DATA FROM CLIENT ] ...");

        received_msg_len = recvfrom(sockfd, message, MAXLINE, 0, (struct sockaddr *)&cliaddr, &len);
        message[received_msg_len] = '\0';
        printf("\n[ FROM CLIENT ]\n-->Received data: %s\n", message);
        for (i = 0; i < received_msg_len; i++)
        {
            if (message[i] == '\n')
            {
                message[i] = '\0';
            }
        }

        int choice;
        char test[MAXLINE], param[MAXLINE];
        strcpy(param, message);
        strcpy(test, param);
        if (isdigit(test[0]))
        {
            choice = 1;
        }
        else
        {
            choice = 0;
        }

        if (choice == 1)
        {
            if (!is_valid_ip(test))
            {
                flag = INVALID_ADDRESS;
            }
            else
            {
                resolve_domain_name_from_ip(param);
            }
        }
        else
        {
            resolve_ip_from_domain_name(param);
        }

        if (flag == NOT_FOUND_INFORMATION)
        {
            strcpy(res, "Not found information\n");
        }
        if (flag == INVALID_ADDRESS)
        {
            strcpy(res, "Invalid address\n");
        }

        printf("\n[ TO CLIENT ]\n-->Data: %s", res);
        sendto(sockfd, res, strlen(res), 0, (struct sockaddr *)&cliaddr, len);
        strcpy(res, "");
        for (i = strlen(message) - 1; i >= 0; i--)
        {
            message[i] = '\0';
        }
    }
    // terminate socket
    close(sockfd);
}
