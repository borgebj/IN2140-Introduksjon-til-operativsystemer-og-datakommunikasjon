//
// Created by borge on 05.02.2024.
//
#include <stdio.h>

// må forhåndsdefinere hvis greet er i annen fil
void greet(char *);

int main(int argc, char *argv[]) {

    // kaller funksjon fra annen fil func.c
    if (argc < 2) {
        printf("usage: %s, <name>\n", argv[1]);
        return -1;
    }

    printf("Forste arg: %s\n",argv[0]);

    greet(argv[1]);
    return 0;
}