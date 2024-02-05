//
// Created by BBJ on 22.01.2024.
//
#include <stdio.h>

void funk(int arr[]) {
    printf("%ld\n", sizeof(arr));
    // gir 8 for int array[11] ??
    // svar: int arr[] er det samme som int *arr  - en peker er 8 byte
}

int main(void) {

    int array[11];
    printf("%llu\n", sizeof(array));

    funk(array);

    return 0;
}