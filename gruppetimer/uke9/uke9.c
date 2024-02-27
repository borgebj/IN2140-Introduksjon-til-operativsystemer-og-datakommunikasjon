//
// Created by borge on 27.02.2024.
//
#include <stdio.h>
#include <stdbool.h> // library for boolean

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

int main(void) {
    unsigned char = 1 << 7;
}