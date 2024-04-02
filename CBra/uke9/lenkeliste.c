//
// Created by BBJ on 02.04.2024.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

struct person {
    int alder;
    char *navn;
    struct person *next;
};

struct node {
    // void peker = peker til hva som helst
    void *data;
    struct node *next;
};

void print_list(struct person *liste) {
    while (liste != NULL) {
        printf("%s er %d aar gammel\n", liste->navn, liste->alder);
        liste = liste->next;
    }
}

int main(void) {

    // på stacken
    struct person p;
    p.alder = 39;
    p.navn = "Olav";

    // på heapen
    struct person *p2 = malloc(sizeof(struct person));
    if (p2 == NULL) {
        fprintf(stderr, "Malloc failed\n");
        return EXIT_FAILURE;
    }
    p2->alder = 12;
    p2->navn = "Gaute";

    p.next = p2;
    p2->next = NULL;

    print_list(&p);

    free(p2); // etter bruk

    return EXIT_SUCCESS;
}