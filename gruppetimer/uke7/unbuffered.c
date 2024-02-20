//
// Created by borge on 13.02.2024.
//
#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>

/* File I/O not using buffer */

#define BUFFSIZE 255


int main(void) {

    // wc: write-counter
    // fc: read-counter
    int fd, wc, rc;
    char buf[BUFFSIZE];

    //
    /* O_WRONLY | O_TRUNC | O_CREAT - 3 forskjellige aksessmetoder*/
    // O_WRONLY = Write-only
    // O_RDWR = Read-write
    fd = open("test.txt", O_RDWR);
    if (fd == -1) {

        // perror og fprintf ganske like, men fprintf skriver til "standard-error"
        // fprintf(stderr, "Open-funksjonen koblet til variabelen fd");
        perror("Open-funksjonen koblet til variabelen fd");
        return EXIT_FAILURE;
    }

    // skriver til fil via write - sjekk om det gikk
    wc = write(fd, "Hallo, det er meg", 17); // <- 17 pga lengden av streng
    if (wc == -1) {
        perror("wc = write()");
        close(fd);
        return EXIT_FAILURE;
    }

    rc = read(fd, buf, BUFFSIZE-1);
    if (rc == -1) {
        perror("Open-funksjonen koblet til variabelen fd");
        close(fd);
        return EXIT_FAILURE;
    }

    buf[rc]= 0;
    printf("%s\n", buf);

    // alltid lukk fil når ferdig!
    // ellers kan minnelekasje oppstå
    close(fd);

    return 0;
}