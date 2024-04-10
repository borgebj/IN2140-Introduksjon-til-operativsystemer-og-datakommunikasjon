//
// Created by BBJ on 09.04.2024.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// sendto
#include <sys/socket.h>
#include <sys/types.h>

// 127.0.0.1
unsigned short port; // 65 000+ porter

int main(void) {

    int sockfd;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    return EXIT_SUCCESS;
}