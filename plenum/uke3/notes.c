//
// Created by borge on 31.01.2024.
//
#include <stdio.h>

struct Coord {
    float x;
    float y;
};
// struct - lager ny type
// inneholder én variabel x
struct S {
    int x;
};
// endrer verdi via referanse i original-struct
void structsub(struct S *p) {
    p -> x = 2;
}
// endrer verdi via referanse
void refsub(int *p) {
    *p = 2;
}
// lager kopi, endrer kopi
void regsub(int p) {
    p = 2;
}
int main() {
    struct S s;
    s.x = 5;
    structsub(&s);
    printf("struct sub: %d\n", s.x);

    int x = 5;
    refsub(&x);
    regsub(x);
    printf("int sub: %d\n", x);

    // koordinater
    struct Coord first;
    first.x = 100;
    first.y = 90;
    printf("(%.2f, %.2f)", first.x, first.y);

    return 0;
}