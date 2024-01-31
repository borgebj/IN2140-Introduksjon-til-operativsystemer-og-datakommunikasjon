//
// Created by BBJ on 22.01.2024.
//
#include <stdio.h>

int main(void) {

    int tall = 5;
    int *peker;
    peker = &tall;

    int nytt_tall = *peker;

    printf("%p\n", &peker); // printer tall
    printf("%p\n", peker);  // printer adressen til tall
    printf("%d\n", nytt_tall);

    // øker verdi til tall vha minne-peker
    // tall: 5 -> tall: 7
    *peker += 2;
    printf("%d\n", *peker);

    return 0;
}