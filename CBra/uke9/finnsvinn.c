//
// Created by BBJ on 02.04.2024.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <zconf.h>

// må ha:
// navn
// beholdning
// registrerte (varer inn - varer ut)

struct vare {
    int beholdning;
    int registrert_inn_ut;
    char *navn;
    struct vare *neste;
};

enum type {
    BEHOLDNING,
    LEVERING,
    SALG
};

// global lenkeliste avv varer
struct vare *varer = NULL;

// frigjør en spesifikk vare
void free_vare(struct vare *v) {
    free(v->navn);
    free(v);
}

// frigjør hele globale listen av varer
void free_varer() {
    struct vare *tmp, *tmp2;

    for (tmp = varer; tmp != NULL; tmp = tmp2) {
        tmp2 = tmp->neste;
        free_vare(tmp);
    }}

// printer en spesifikk vare
void print_vare(struct vare *v) {
    int svinn = v->registrert_inn_ut - v->beholdning;
    printf("Svinn: %d : %s\n", svinn, v->navn);
}

// printer alle varer
void print_varer() {
    struct vare *tmp;

    for (tmp = varer; tmp != NULL; tmp = tmp->neste) {
        print_vare(tmp);
    }
}

void lag_vare(char *navn, unsigned char antall, enum type t) {
    struct vare *v = malloc(sizeof(struct vare));

    if (v == NULL) {
        fprintf(stderr, "malloc failed\n");
        free_varer();
        exit(EXIT_FAILURE);
    }

    v->navn = strdup(navn); // alloker og kopier
    if (v->navn == NULL) {
        perror("strdup");
        free_varer();
        free(v);
        exit(EXIT_FAILURE);
    }

    v->beholdning = 0;
    v->registrert_inn_ut = 0;

    switch (t) {
        case BEHOLDNING:
            v->beholdning = antall;
            break;
        case LEVERING:
            v->registrert_inn_ut = antall;
            break;
        case SALG:
            v->registrert_inn_ut = -antall;
            break;
    }

    // setter vare bak head i listen, bytter head
    v->neste = varer;
    varer = v;
}

void legg_til_vare(char *navn, unsigned char antall,  enum type t) {
    struct vare *tmp;

    // sjekker om vare finnes fra før
    tmp = varer;
    while (tmp != NULL) {

        // hvis ulik: gå til  neste  |  0 = unequal
        if (strcmp(tmp->navn, navn) != 0) {
            tmp = tmp->neste;
            continue;
        }
        switch (t) {
            case BEHOLDNING:
                tmp->beholdning = antall;
                return;
            case LEVERING:
                tmp->registrert_inn_ut += antall;
                return;
            case SALG:
                tmp->registrert_inn_ut -= antall;
                return;
        }
        tmp = tmp->neste;
    }
    // hvis ikke finnes: lag ny
    lag_vare(navn, antall, t);
}

// leser beholdning
void les_log(char *filnavn, enum type t) {
    FILE *fil = NULL;
    char navn[256]; // buffer for innlesing
    unsigned char antall, navn_len;
    int rc;

   fil = fopen(filnavn, "rb");

    // error-check
    if (fil == NULL) {
        perror("fopen");
        free_varer();
        exit(EXIT_FAILURE);
    }

    // Innlesing
    while ((rc = fread(&antall, sizeof(char), 1, fil))) {
        if (fread(&navn_len, sizeof(char), 1, fil) == 0) break;
        rc = fread(navn, sizeof(char), navn_len, fil);
        if (rc != navn_len) break;
        navn[navn_len] = '\0';

        legg_til_vare(navn, antall, t);
    }

    if (ferror(fil)) {
        fprintf(stderr, "fread failed\n");
        free_varer();
        exit(EXIT_FAILURE);
    }

    if (fclose(fil) == EOF) {
        perror("fclose");
        free_varer();
        exit(EXIT_FAILURE);
    }
}

int main(void) {
    les_log("salg.log", SALG);
    les_log("levering.log", LEVERING);
    les_log("beholdning.log", BEHOLDNING);
    print_varer();
    free_varer();

    return EXIT_SUCCESS;
}