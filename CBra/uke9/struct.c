//
// Created by BBJ on 02.04.2024.
//
#include <stdlib.h>
#include <stdio.h>

struct MyStructure { // struct declaration
    int myNum;       // member (int var)
    char myLetter;   // member (char var)
};                   // struct end (semicolon)

int main(void) {

    struct MyStructure struktur = {7, 'B'};
    struct MyStructure struktur2;

    struktur2 = struktur;
    struktur.myNum = 7;
    struktur.myLetter = 'B';

    return EXIT_SUCCESS;
}