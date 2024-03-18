
// standard + error
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

// Ip + socket
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h> // close

#define PORT 2053

// error check for socket
void check_error(int val, char *desc) {
    if (val == -1) {
        perror(desc);
        exit(-1);
    }
}

int main()
{
    char buff[255];

    int sockfd;
    int rc;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    check_error(sockfd, "socket");

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    rc = bind(sockfd, (const struct sockaddr *) &server_address, sizeof(server_address));
    check_error(rc, "bind");

    // -1 pga null-byte     | receive |
    rc = recv(sockfd, buff, 255-1, 0);
    check_error(rc, "receive");
    buff[rc] = 0;

    printf("%s\n", buff);

    close(sockfd);
    return 0;
}