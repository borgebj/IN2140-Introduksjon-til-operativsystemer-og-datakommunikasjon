//
// Created by BBJ on 22.01.2024.
//
#include <stdio.h>

// kan aksesseres overralt
int global;

void min_funksjon() {
    int lokal;
    printf("Lokal: %p\n", &lokal);  // 000000DaF47FF9CC
}

int main(void) {
    int tall;

    printf("Global: %p\n", &global); // 00007FF7D3C190A0
    printf("Main: %p\n", &tall);  // 000000Af47FFA0C
    min_funksjon();

    // global ligger lenger nede i layout
    return 0;
}