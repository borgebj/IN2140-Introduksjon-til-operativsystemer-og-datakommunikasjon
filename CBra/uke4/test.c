//
// Created by BBJ on 07.02.2024.
//
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// lager kopi av original med malloc. Må frees av den som kaller etter bruk
char *lagKopi(char *original) {

    int lengde = strlen(original) + 1;
    char *kopi = malloc(sizeof(char) * lengde);

    // sjekker om malloc lykkes
    if (kopi == NULL) {
        fprintf(stderr, "malloc: feil i lagKopi\n");
        exit(-1);
    }
    char *start =  kopi;

    // metode 1 - for-løkke
    for (int i=0; i < lengde; i++) {
        kopi[i] = original[i];
    }

    // metode 2 - while m/ pointer
    while (*original) {
        *kopi++ = *original++;
    }

    while (*original != 0) {
        *kopi = *original;
        kopi++;
        original++;
    }

    // returner start siden kopi peker på slutten
    return start;
}

int main(void) {

    char *streng = "Hei Cyber-Cbra!";
    char *kopi = lagKopi(streng);

    printf("Her er kopi: %s\n", kopi);

    free(kopi);
    return 0;
}