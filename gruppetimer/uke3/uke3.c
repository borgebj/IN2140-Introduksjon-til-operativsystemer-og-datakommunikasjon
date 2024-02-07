//
// Created by borge on 30.01.2024.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {

    // allokerer 20 minne-plasser for navn
    char* navn = malloc(20);

    if (navn == NULL) { printf("Malloc feil"); return EXIT_FAILURE; }

    strcpy(navn, "Ola Nordmann");  // kopiere "Ola Nordmann" inn i strengen navn

    printf("%s\n", navn);

    while (*navn != '0') {
        printf("%c", *navn++);
    }

    return 0;
}