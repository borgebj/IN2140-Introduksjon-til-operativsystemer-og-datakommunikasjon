//
// Created by BBJ on 27.02.2024.
//
#include <stdio.h>
#include <string.h>

char* toLowerCase(char *str) {
    char *lower = str;
    while (*str != '\0') {
        *(str++) |= 0b00100000;
    }
    return lower;
}

void oppgaveEn(void) {

    // 1.1 ----------------------------------
    unsigned char bokstaver[] = {195, 226, 242, 225, 161, 128};
    int mask = 0b01111111; // same as: (16) 0x7F or (10) 127

    // & (AND) - "ganger" to bits sammen

    for (int i = 0; i < 6; ++i) {
        char bokstav = (char) (bokstaver[i] & mask);
        printf("%c", bokstav);
    } printf("\n");

    // 1.2 ----------------------------------
    char str_en[] = "HELloCBrA";
    char b = 0b00100000; // tilsvarer 32
    // (character) | 32 tilsvarer lowercase
    // (F) = 70. 70 | 32 = 01000110 | 00100000 = 01100110

    // | (OR) - "plusser" to bits sammen

    for (int i = 0; i < strlen(str_en); ++i) {
        char bokstav = (char) (str_en[i] | b);
        printf("%c", bokstav);
    } printf("\n");

    char name[] = "BORGE";
    printf("%s\n", toLowerCase(name));

    // 1.3 ----------------------------------
    char str_to[] = "HELloCBrA";
    char bb = 0b00100000; //  tilsvarer 32

    // ^ (XOR) - "ekslusivt or" - 1 hvis ulike, 0 hvis like
    // dvs. (1 0 = 1), (1 1 = 0), (0 0 = 0), (0 1 = 1)

    char str_tre[] = "HELloCBrA";
    char bbb = 0b100000;
    for (int i = 0; i < strlen(str_tre); ++i) {
        char bokstav = (char) (str_tre[i] ^ bbb);
        printf("%c", bokstav);
    } printf("\n");

    // 1.4 ----------------------------------
    unsigned char bokstaver_to[] = {156, 189, 173, 190, 255};
    for (int i = 0; i < 5; ++i) {
        char bokstav = (char) ~bokstaver_to[i];
        printf("%c", bokstav);
    } printf("\n");
}

int main(void) {
    oppgaveEn();
    return 0;
}