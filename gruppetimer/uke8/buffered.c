//
// Created by borge on 13.02.2024.
//
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

/* File I/O using buffer */

#define BUFFSIZE 255

int main(void) {
//    FILE *fil;
    char* msg = "Hallo det er meg";
    char buf[BUFFSIZE];

    size_t wc;
    size_t rc;

    FILE *fil = fopen("new_file.txt", "r"); // en biblioteksfunksjon

    wc = fwrite(msg, sizeof(char), strlen(msg), fil);
    if (wc < strlen(msg)) {
        perror("Feil i fwrite");
        fclose(fil);
        return EXIT_FAILURE;
    }

    rc = fread(buf, sizeof(char), BUFFSIZE-1, fil);

    if (rc < strlen(msg) && ferror(fil)) {
        fprintf(stderr, "Feil i fread()\n");
        fclose(fil);
        return EXIT_FAILURE;
    }

    buf[rc]= 0;
    printf("%s\n", buf);

    // lukker allitd fil etter ferdig bruk
    // ellers kan minnelekasje skje
    fclose(fil);
}