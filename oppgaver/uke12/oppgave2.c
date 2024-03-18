//
// Created by BBJ on 18.03.2024.
//
#include <stdlib.h>
#include <stdio.h>

#include <arpa/inet.h> // sockaddr_in
#include <unistd.h> // close
#include <string.h> // memset

#include <sys/types.h>
#include <sys/socket.h> // socket og bind
//     int socket(int domain, int type, int protocol)
//     int bind(int sockfd, const struct sockaddr *addr, socklen-t addrlen)

#define PORT 2023
#define BUFSIZE 250
#define TERMINATION_MSG "q"

// trenger info (server):
// 1. Socket som mottar datagrammer
// 2. Informasjon om hvilken IP addresse
// 3. Port man vil lytte på

void check_error(int val, char *desc);

int main() {

    char buffer[BUFSIZE] = { 0 }; // initialiserer med null-bytes

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    check_error(sockfd, "socket");

    // ip addresse
    struct in_addr ip_address;
    inet_pton(AF_INET, "127.0.0.1", &ip_address);

    // port info
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    address.sin_addr = ip_address;

    // binds socket
    int ret = bind(sockfd, (struct sockaddr *)&address, sizeof(struct sockaddr_in));
    check_error(ret, "bind");

    int rc = 0;
    while(rc != -1) {
        // received message - man 2 recv
        rc = recv(sockfd, buffer, BUFSIZE - 1, 0); //     ssize_t recv(int sockfd, void *buf, size_t len, int flags)
        check_error(rc, "receive");
        buffer[rc] = '\0';

        // prints message
        printf("%s\n", buffer);

        // clears buffer - resets string message
        memset(buffer, 0, BUFSIZE);
    }

    close(sockfd);
    return 0;
}

void check_error(int val, char *desc) {
    if (val == -1) {
        perror(desc);
        exit(-1);
    }
}