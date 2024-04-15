//
// Created by BBJ on 09.04.2024.
//
#include "common.h"

void check_error(int res, char *msg) {
    if (res == -1) {
        perror(msg);
        exit(-1);
    }
}

int main(void) {

    int sockfd, rc, wc;
    char buff[255];
    struct sockaddr_in my_addr, src_addr;
    socklen_t addr_len;
    char *reply = "Hei tilbake fra server";

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    check_error(sockfd, "socket");

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET; // AF_INET for TCP/UDO
    server_addr.sin_port = htons(PORT); // sikrer riktig endianess
    server_addr.sin_addr.s_addr = INADDR_ANY; // any : lytter på alt

    rc = bind(sockfd,
              (const struct sockaddr *) &server_addr,
                      sizeof(server_addr));
    check_error(rc, "bind");

    addr_len = sizeof(struct sockaddr_in);
    rc = recvfrom(sockfd,
                  buff,
                  255-1,
                  0,
                  (struct sockaddr *) &src_addr,
                    &addr_len);
    check_error(rc, "receive");
    buff[rc] = 0;

    printf("%s\n", buff);

    wc = sendto(sockfd,
                reply,
                strlen(reply),
                0,
                (struct sockaddr*) &src_addr,
                sizeof(struct sockaddr_in));
    check_error(wc, "sendto");

    close(sockfd);
    return EXIT_SUCCESS;
}