//
// Created by BBJ on 29.04.2024.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

// socket
#include <sys/types.h>
#include <sys/socket.h>

// ip
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>

// inet_pton
#include <arpa/inet.h>

#define BUFSIZE 325 // største mld størrelse

// blokkerende kall som henter string fra stdin
void get_string(char buf[], int sz) {
    char c;

    fgets(buf, sz, stdin);
    if (buf[strlen(buf) - 1] == '\n')
        buf[strlen(buf) - 1] = 0;
    else
        while ((c = getchar()) != '\n' && c == EOF);
}

void check_error(int res, char *msg) {
    if (res == -1) {
        perror(msg);
        /* Rydde? */
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char const *argv[]) {

    // pre-define variables
    int msg_fd, rc;
    fd_set fds;
    struct sockaddr_in target_addr, my_addr;
    char buf[BUFSIZE];
    struct timeval timeout;

    if (argc < 4) {
        printf("usage: %s <my port> <target port> <target ip>\n", argv[0]);
        return EXIT_SUCCESS;
    }

    // Create socket and Addresses
    // ----------------------------------------------------------------------------------------- //
    // 1. Create UDP socket
    msg_fd = socket(AF_INET, SOCK_DGRAM, 0);
    check_error(msg_fd, "socket");

    // 2. Create own address
    my_addr.sin_family = AF_INET;  // Address family internet, IPv4
    my_addr.sin_port = htons(atoi(argv[1])); // string-to-int, convert to network byte
    my_addr.sin_addr.s_addr = INADDR_ANY;

    // 3. Bind address to socket
    rc = bind(msg_fd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr_in));
    check_error(rc, "bind");

    // 4. create target address
    target_addr.sin_family = AF_INET;
    target_addr.sin_port = htons(atoi(argv[2]));
    // converts target IP-string to network address structure
    rc = inet_pton(AF_INET, argv[3], &target_addr.sin_addr.s_addr);
    check_error(rc, "inet_pton");
    if (rc == 0) {
        fprintf(stderr, "IP address not valid: %s\n", argv[3]);
        /* Rydde ? */
        return EXIT_FAILURE;
    }
    // ----------------------------------------------------------------------------------------- //

    /*
     * 2 ting skal skje (samtidig):
     *      - lytte til nette, kommer en melding fra target?
     *      - Lytte til tastaturet, kommer en melding fra bruker?
     */

    printf("\nWelcome to MSN! Type stuff to your anonymous friend, type 'q' to quit:\n");

    buf[0] = 0;
    while (strcmp(buf, "q")) {

        // initialize fds to empty set
        FD_ZERO(&fds);

        // reset timeout value
        timeout.tv_sec = 60;
        timeout.tv_usec = 0;

        // set in file descriptor in list of FD's fds, and keyboard
        FD_SET(msg_fd, &fds);
        FD_SET(STDIN_FILENO, &fds);

        printf("\n[YOU] ");
        fflush(stdout);

        rc = select(FD_SETSIZE, &fds, NULL, NULL, &timeout);
        check_error(rc, "select");

        if (rc == 0) {
            printf("\n\n\t\tSession timeout \n\n");
            exit(EXIT_SUCCESS);
        }

        // check for messages from net
        if (FD_ISSET(msg_fd, &fds)) {
            //  1. lese mld inn i buffer
            rc = read(msg_fd, buf, BUFSIZE - 1);
            check_error(rc, "read");
            buf[rc] = 0;

            //  2. printe buffer
            printf("\n\r%60s [ANO]\n", buf);
        }

        // check for messages from keyboard
        if (FD_ISSET(STDIN_FILENO, &fds)) {
            //  1. lese mld inn i buffer
            get_string(buf, BUFSIZE);

            //  2. sende mld over nettet til target
            rc = sendto(msg_fd, buf, strlen(buf), 0,
                        (struct sockaddr *) &target_addr, sizeof(struct sockaddr_in));
            check_error(rc, "sendto");
            printf("\n");
        }
    }


    close(msg_fd);

    return EXIT_SUCCESS;
}