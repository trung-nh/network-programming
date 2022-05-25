#include <stdio.h>  //printf
#include <string.h> //memset
#include <stdlib.h> //for exit(0);
#include <sys/socket.h>
#include <errno.h> //For errno - the error number
#include <netdb.h> //hostent
#include <arpa/inet.h>
#include <ctype.h>

int hostname_to_ip(char *, char *);
int is_valid_ip(char *ip);
int is_digit_string(char *str);
void resolve_domain_name_from_ip(char *ip);
void resolve_ip_from_domain_name(char *name);

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Please provide a domain name or IP address to resolve !");
        exit(1);
    }
    char *param = argv[1];
    char ip[100];
    if (isdigit(param[0]))
    {
        if (!is_valid_ip(param))
        {
            printf("IP address malformed!\n");
            exit(0);
        }
        resolve_domain_name_from_ip(param);
    }
    else
    {
        resolve_ip_from_domain_name(param);
    }
}

int hostname_to_ip(char *hostname, char *ip)
{
    struct hostent *he;
    struct in_addr **addr_list;
    int i;

    if ((he = gethostbyname(hostname)) == NULL)
    {
        // get the host info
        herror("gethostbyname");
        return 1;
    }

    addr_list = (struct in_addr **)he->h_addr_list;

    for (i = 0; addr_list[i] != NULL; i++)
    {
        // Return the first one;
        strcpy(ip, inet_ntoa(*addr_list[i]));
        printf("\nOfficial Name : %s\n", he->h_name);
        printf("\nOfficial Name : %s\n", *(he->h_aliases));
        return 0;
    }

    return 1;
}

void ip_to_hostname(char *ip, char *buffer)
{
    struct sockaddr_in sa; /* input */
    socklen_t len;         /* input */

    memset(&sa, 0, sizeof(struct sockaddr_in));

    /* For IPv4*/
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = inet_addr(ip);
    len = sizeof(struct sockaddr_in);

    if (getnameinfo((struct sockaddr *)&sa, len, buffer, sizeof(buffer),
                    NULL, 0, NI_NAMEREQD))
    {
        printf("Resolution failed!\n");
    }
}

int is_digit_string(char *str)
{
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
    struct in_addr addr;
    int i;
    inet_aton(ip, &addr);
    struct hostent *he = gethostbyaddr(&addr, sizeof(addr), AF_INET);
    if (he == NULL)
    {
        printf("Resolution failed!\n");
        exit(1);
    }

    // happy case
    printf("Official name: %s\n", he->h_name);
    printf("Alias name:\n");
    if (he->h_aliases[0] != NULL)
    {
        for (i = 0; he->h_aliases[i] != NULL; i++)
        {
            printf("%s", he->h_aliases[i]);
        }
    }
    else
    {
        printf("No alias name resoved!\n");
    }
}
void resolve_ip_from_domain_name(char *name)
{
    struct hostent *he = gethostbyname(name);
    int i;
    if (he == NULL)
    {
        printf("Resolution failed!\n");
        exit(1);
    }

    // happy case
    printf("Official IP: %s\n", inet_ntoa(*(struct in_addr *)he->h_addr));
    printf("Alias IP(s):\n");
    struct in_addr **addr_list = (struct in_addr **)he->h_addr_list;
    if (addr_list[0] != NULL)
    {
        for (i = 0; addr_list[i] != NULL; i++)
        {
            printf("%s\n", inet_ntoa(*addr_list[i]));
        }
    }
    else
    {
        printf("No alias IP resolved!\n");
    }
}