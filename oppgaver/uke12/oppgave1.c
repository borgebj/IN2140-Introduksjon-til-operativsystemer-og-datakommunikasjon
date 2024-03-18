
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>

#define PORT 2023

void check_error(int val, char *desc);


int main() {

    char *msg;

    // socket
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    check_error(sockfd, "socket");

    // ip addresse
    struct in_addr ip_address;
    inet_pton(AF_INET, "127.0.0.1", &ip_address);

    // portnummer
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    address.sin_addr = ip_address;

    msg = "Cbra er gøy!";
    sendto(sockfd,
           msg,
           strlen(msg),
           0,
           (struct sockaddr*)&address,
           sizeof(struct sockaddr_in));

    return 0;
}

// error check
void check_error(int val, char *desc) {
    if (val == -1) {
        perror(desc);
        exit(-1);
    }
}