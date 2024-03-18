
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

#define PORT 2053
#define IP_ADDRESS "127.0.0.1"  // <-- reservert addresse til seg selv

// error check for socket
void check_error(int val, char *desc) {
    if (val == -1) {
        perror(desc);
        exit(-1);
    }
}

int main()
{
    char melding[] = "Hei fra Børge!";

    // man 2 socket | man 7 ip
    // lager UDP socket
    int sockfd;
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    check_error(sockfd, "socket");

    // lager sockaddr "interface struct" som beskriver addressen vi sender til
    struct sockaddr_in dest_addr;
    struct in_addr addr;
    int wc = inet_aton(IP_ADDRESS, &addr);
    if (wc == 0) { perror("inet_aton"); exit(-1); }

    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(PORT);
    dest_addr.sin_addr = addr;

    // sender melding med UDP     | send |
    sendto(sockfd,
           melding,
           strlen(melding),
           0,
           (const struct sockaddr *) &dest_addr,
           sizeof(struct sockaddr_in));
    // trenger også struct lengde på slutt pga ipv4 og ipv6 f.eks har stor forskjell

    check_error(wc, "sendto");

    return 0;
}