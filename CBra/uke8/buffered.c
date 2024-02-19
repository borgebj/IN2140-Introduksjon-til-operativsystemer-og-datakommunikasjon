//
// Created by BBJ on 19.02.2024.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 255 // tilfeldig

int main(void) {

    // create and open file
    FILE *fil;

    if ((fil = fopen("ny_fil.txt", "w+")) == NULL) { // error-check: open
        perror("fopen failed");
        return EXIT_FAILURE;
    }

    // write to file
    // (trenger forhåndslagd melding)
    char *msg = "dette er en buffered write";
    size_t wc = fwrite(msg, sizeof(char), strlen(msg), fil);
    if (wc < strlen(msg)) { // error-check: write
        fprintf(stderr, "fwrite: short item count\n");
        fclose(fil);
        return EXIT_FAILURE;
    }

    // rewind file pointer to beginning (ikke i CBra)
    rewind(fil);

    // read file
    // (trenger buffer mellomlagring)
    char buf[BUFSIZE];
    size_t rc = fread(buf, sizeof(char), BUFSIZE, fil);
    if (rc < BUFSIZE && ferror(fil)) {
        fprintf(stderr, "fread: error occured\n");
        fclose(fil);
        return EXIT_FAILURE;
    }
    buf[rc] =  '\0';

    fprintf(stdout, "buf: %s\n", buf);

    // lukker fil
    fflush(fil); // force flush
    fclose(fil);
    return EXIT_SUCCESS;
}