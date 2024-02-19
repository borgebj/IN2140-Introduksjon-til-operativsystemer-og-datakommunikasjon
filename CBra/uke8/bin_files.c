//
// Created by BBJ on 19.02.2024.
//

/*
 * Reading non-character data
 */

#include <stdio.h>
#include <stdlib.h>

#define ARRSIZE 4

int main(void) {

    FILE *file;
    unsigned int array[] = {1, 16, 64, 255};
    size_t wc;

    if ((file = fopen("my_bin.min_ext", "wb+")) == NULL) {
        perror("fopen");
        return EXIT_FAILURE;
    }

    wc = fwrite(array, sizeof(int), ARRSIZE, file);
    if (wc < ARRSIZE) {
        fprintf(stderr, "fwrite: short item count\n");
        fclose(file);
        return EXIT_FAILURE;
    }

    rewind(file);

    unsigned int buf[ARRSIZE];
    size_t rc = fread(buf, sizeof(int), ARRSIZE, file);
    if (rc < ARRSIZE && ferror(file)) {
        fprintf(stderr, "fread: error occured\n");
        fclose(file);
        return EXIT_FAILURE;
    }

    for (size_t i = 0; i < ARRSIZE; i++) {
        printf("buf %zu: %d\n", i, buf[i]);
    }

    fclose(file);
    return EXIT_SUCCESS;
}