//
// Created by borge on 14.02.2024.
//
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFSIZE 100

int main(void) {
    int fd, wc;
    char buf[BUFSIZ]; // buffer som holder 100 bytes

    // opens file-descriptor
    // O_CREAT lager fil om ikke finnes
    // O_TRUNC respawner fil om finnes
    fd = open("text.txt", O_WRONLY | O_TRUNC | O_CREAT);
    if (fd == -1) {
        perror("open failed");
        return EXIT_FAILURE;
    }

    wc = write(fd, "hei", 3);
    if (wc == -1) {
        perror("write failed");
        close(fd);
        return EXIT_FAILURE;
    }

    // closes file
    close(fd);

    return EXIT_SUCCESS;
}