//
// Created by BBJ on 19.02.2024.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {

    // buffered
    fprintf(stdout, "Hello to ");
    sleep(1);
    fprintf(stdout, "Buffered \n");

    // printf("hei"); = fprintf(stdout, "hei");

    sleep(1);

    // unbuffered
    fprintf(stderr, "Hello to ");
    sleep(1);
    fprintf(stderr, "Unbuffered\n");

    return EXIT_SUCCESS;
}