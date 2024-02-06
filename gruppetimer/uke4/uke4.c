//
// Created by borge on 06.02.2024.
//
#include <stdio.h>
#include <stdlib.h> // Malloc
#include <string.h> // strdup

struct bil {
    char* regNr;
    char* type;
    int prodYear;
};
// Makroer - forkortelser av typer
typedef struct bil Bil;
typedef unsigned char uchar;

void legg_til_verdier_struct(struct bil *b, int p, char* r) {

    // bil fra parameter er pointer, og kalles med & (reference)
    // derfor blir originale bilen overskrevet/satt via ->

    // dereference
    b -> prodYear = p;
    b -> regNr = r;
}

void printBil(struct bil* b) {
    printf("Registreringsummer: %s\nProduction year: %d\n", b->regNr, b->prodYear);
}

struct bil *lag_bil(int p, char* regNr) {

    // allokere minne på størrelse til bil
    struct bil *car = malloc((sizeof(struct bil)));

    // hvis minneallokering misslykket:
    if (car == NULL) {
        printf("Malloc failed ...");
        exit(1); // terminerer alt
    }

    // basically strdup --------------
//    char *var = malloc(10);
//    strcpy(var, "hello");
    // -------------------------------

    // bruker strdup for å duplikere over regNr - allokerer minne
    // direkte skrive inn aka regNr = regNr, allokerer ikke minne   (kan overskrive data(?))
    car -> regNr = strdup(regNr);
    car -> prodYear = p;

    // må huske å free(bil -> regNr) senere pga strdup

    return car;
}

// frigjør minne til en spesifikk bil
void free_car(struct bil* b) {
    free(b -> regNr);
    free(b);
}

int main(void) {

    Bil ford;
    ford.regNr = "AB 12345";
    ford.type = "Ford";
    ford.prodYear = 2022;
    printBil(&ford);

    struct bil ferrari;
    legg_til_verdier_struct(&ferrari, 2022, "B8223");

    // lager bil via malloc og dereferencing
    struct bil* ny_bil = lag_bil(2023, "AB 5532");
    printBil(ny_bil);

    // pga malloc() og strdup() har vi allokert minne    - som skjer i lag_bil
    // må dermed frigjøre minnet for å unngå lekasje
    free_car(ny_bil);
}