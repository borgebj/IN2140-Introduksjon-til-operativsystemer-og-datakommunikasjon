//
// Created by BBJ on 09.04.2024.
//
#include "common.h"

// 127.0.0.1
unsigned short port; // 65 000+ porter

void check_error(int res, char *msg) {
    if (res == -1) {
        perror(msg);
        exit(-1);
    }
}

int main(void) {

    int sockfd, wc, rc;
    char *msg = "Cbra sender over localhost";
    char buff[255];

    // lager udp socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    check_error(sockfd, "socket");

    // lagrer addresse og port
    struct sockaddr_in dest_addr;
    struct in_addr ip_addr;

    // konvertere IP fra string til bytes / int
    wc = inet_pton(AF_INET, IP, &ip_addr.s_addr); // sett inn IP
    check_error(wc, "inet_pton");
    if (!wc) {
        fprintf(stderr, "Invalid IP address %s\n", IP);
        return EXIT_FAILURE;
    }

    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(PORT); // i network byte-order
    dest_addr.sin_addr = ip_addr;

    wc = sendto(sockfd,
                msg,
                strlen(msg),
                0,
                (const struct sockaddr *) &dest_addr,
                sizeof(dest_addr));
    check_error(wc, "sendto");

    rc = recv(sockfd, buff, 255-1, 0);
    check_error(rc, "receive");
    buff[rc] = 0;

    printf("vi fikk svar: %s\n", buff);

    close(sockfd);
    return EXIT_SUCCESS;
}