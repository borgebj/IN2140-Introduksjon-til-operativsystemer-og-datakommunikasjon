//
// Created by borge on 14.02.2024.
//
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFSIZE 100

/*
 * Grunnen for "unbuffered" er fordi vi bruker open() read() write()
 */

int main(void) {
    int fd, wc, rc;
    char buf[BUFSIZ]; // buffer som holder 100 bytes

    // opens file-descriptor
    // O_CREAT lager fil om ikke finnes
    // O_TRUNC respawner fil om finnes
    fd = open("text.txt", O_RDWR | O_CREAT); // read og write
    if (fd == -1) {
        perror("open failed");
        return EXIT_FAILURE;
    }

    // skriver til fil "hei"
    wc = write(fd, "hei", 3);
    if (wc == -1) {
        perror("write failed");
        close(fd);
        return EXIT_FAILURE;
    }

    // resets cursor pointer to start (ikke med på CBra)
    lseek(fd, 0, SEEK_SET == -1);

    // leser fra fil
    rc = read(fd, buf, BUFSIZE);
    if (rc == -1) {
        perror("read failed");
        close(fd);
        return EXIT_FAILURE;
    }

    // husk å sette null-byte !
    buf[rc] = 0;

    printf("I bufferet ligger det: %s\n", buf);

    // closes file
    close(fd);

    return EXIT_SUCCESS;
}