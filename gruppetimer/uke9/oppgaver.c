//
// Created by BBJ on 27.02.2024.
//
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
        bokstaver[i] = (bokstaver[i] & mask);
    }
    printf("%s\n", (char *) bokstaver);

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

void printbits(void *n, int size) {
    char *num = (char *)n;
    int i, j;

    for (i = size-1; i >= 0; i--) { // itererer gjennom bytes
        for (j = 7; j >= 0; j--) {
            printf("%c", (num[i] & (1 << j)) ? '1' : '0');
        }
        printf(" ");
    }
    printf("\n");
}

void printchar(unsigned char c) {
    printbits(&c, sizeof(char));
}

void print_exec(unsigned char a, unsigned char b, unsigned char c) {
    printf("a: "); printchar(a);
    printf("b: "); printchar(b);
    printf("c: "); printchar(c);
}

void testBits(void) {
    // ---------------------------------------
    unsigned char c = 12;

    printbits(&c, sizeof(char));

    // shift (12) 2 til høyre
    c = 0b1100 >> 2;

    printbits(&c, sizeof(char));

    unsigned char a, b;
    a = 0b01101101; // 109
    b = 0b10111001; // 185
    c = a & b;
    print_exec(a, b, c);
    // ---------------------------------------
}

// ---------------------------------------------------------------------

#define ID_MASK        0b0000000000011111 // aka 0b11111
#define DONATION_MASK  0b1111111100000000
#define GRADE_MASK     0b0000000011100000 // aka 11100000

// 1. lage maske som markerer siste 5 ^
// 2. AND maske med short
// for forkorting -> shift til høyre

unsigned char hent_karakter(unsigned char s) { // shift 5 for å få vekk ID
    return (s & GRADE_MASK) >> 5;
}
unsigned char hent_donasjon(unsigned short s) { // shift 8 for å vekk vekk resten
    return (s >> 8);
}
unsigned char hent_id(unsigned short s) {
    return s & ID_MASK;
}

void print_soknad(unsigned short s) {
    printf("ID %d\nKarakter %d\nDonasjon %d\n", hent_id(s), hent_karakter(s), hent_donasjon(s));
    printf("Score: %d\n\n", hent_karakter(s) * hent_donasjon(s));
}

int is_set(unsigned char flags) {
    return flags & (1 << 5); // = flags & 0b00100000
}

void set_flag(unsigned char *flags) {
    *flags |= (1 << 5);
}

void unset_flag(unsigned char *flags) {
    *flags &= ~(1 << 5);
}

void bitmask() {
    // søknad som inneholder informasjon om søknad på følgende format:
    /*
     * 16 bit til sammen
     * 8 bits - donasjon
     * 3 bits - karakter (gjennomsnitt) 3 bits, opptil 7 tal
     * 5 bits - ID
     */
    unsigned short soknad1 = 0b0000000011001010; // 0 donasjon, 6 karakter, 10 id
    unsigned short soknad2 = 0b1111010000110010; // 244 donasjon, 1 karakter, 18 id

    print_soknad(soknad1);
    print_soknad(soknad2);
}

void flags() {
    unsigned char bil = 0b01001101;

    set_flag(&bil);
    printf("is set: %s\n", (is_set(bil) > 0 ? "yes" : "no"));
    unset_flag(&bil);
    printf("is set: %s\n", (is_set(bil) > 0 ? "yes" : "no"));
}
// ---------------------------------------------------------------------


int main(void) {
    printf("\nOppgave en\n"); oppgaveEn();
    printf("\nTest bits\n"); testBits();
    printf("\nBitmask\n"); bitmask();
    printf("\nFlags\n"); flags();
    return EXIT_SUCCESS;
}