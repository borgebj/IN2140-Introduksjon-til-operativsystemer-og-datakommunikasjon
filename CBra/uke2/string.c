//
// Created by BBJ on 22.01.2024.
//
#include <stdio.h>

int main(void) {

    // måte 1
    char tom_streng[4];    // tom streng på lengde 4
    tom_streng[0] = 'h';
    tom_streng[1] = 'e';
    tom_streng[2] = 'i';
    tom_streng[3] = 0;
    // stort sett identisk til 2
    // 2 inkluderer null-terminator, 1 må gjøres selv

    // måte 2
    char streng[] = "hei";
    streng[1] = 'a';   // lov - streng ligger på stacken

    // måte 3
    char *s = "hei";
    // s[1] = 'a';     // ikke lov - "read-only"

    // måte 4
    char string_arr[] = {'h', 'e', 'i', 0};

    printf("%s %s %s %s\n", streng, s, tom_streng, string_arr);

    // null-byte eksempel
    char streng2[] = "hei og haa";
    streng2[5] = 0;

    // peker til streng etter
    char *rest1 = streng2 + 6;
    char *rest2 = &streng2[6];

    printf("%s\n", streng2);
    printf("rest: %s %s", rest1, rest2);

    return 0;
}