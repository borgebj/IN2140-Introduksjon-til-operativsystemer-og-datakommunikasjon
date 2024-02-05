//
// Created by borge on 05.02.2024.
//
#include <stdio.h>

int main(void) {
    unsigned int i;

//  assignment i sannhets-sjekk er lov i C !
//  dvs dette vil aldri printes
//  if (i = 0)
    if (i == 0) {
        printf("Hello!\n");
    } else {
        printf("Hey!\n");
    }

    return 0;
}