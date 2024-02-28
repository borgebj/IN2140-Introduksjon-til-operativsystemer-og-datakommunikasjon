//
// Created by borge on 28.02.2024.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// seek time dominant factor of total disk I/O
// this: letting OS or disk controller choose which reqest to serve ...
// ... depending on heads current position ad requested blocks positon on disk

// FCSC - First come first serve
// choosing requests based on initial order
// e.g. starting with | 7 2 9 4 3 2 | start from 7 to 2, no change of order

#define DISKSIZE 40

enum DSAlgos {
    NONE,
    FCFS,
    LOOK,
    LOOKDOWN,
    SCAN
};

typedef enum DSAlgos DSAlgos;

// scheduling algorithms
int fcfs( char *tracks, int pos, int *requests, int num);
int look( char *tracks, int pos, int *requests, int num);
int lookdown( char *tracks, int pos, int *requests, int num);
int scan (char *tracks, int pos, int *requests, int num);

// custom compare function for quicksort
int lt( const void *l, const void *r);


int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("usage: %s <algo> <track\nwhere\n"
               "<algo> is FCSC or LOOK (starting upwards) or LOOKDOWN (starting downwards) or SCAN\n"
               "<track> is an integer indicating a track number\n"
               "means that we must have a tleast one track.\n", argv[0]);
        exit(-1);
    }
    DSAlgos algo;
    if (strncmp(argv[1], "FCFS", 5) == 0) {
        algo = FCFS;
    } else if (strncmp(argv[1], "LOOK", 5) == 0) {
        algo = LOOK;
    } else if (strncmp(argv[1], "LOOKDOWN", 5) == 0) {
        algo = LOOKDOWN;
    } else if (strncmp(argv[1], "SCAN", 5) == 0) {
        algo = SCAN;
    } else {
        printf("Unsupported algorithm\n");
        exit(-1);
    }
    // strcmp(str1, str2) = string compare
    // strncmp(str1, str2, #) = string compare med max-lengde på #

    int num_requests = argc-2 ;
    int *requests = malloc(num_requests * sizeof(int));
    if (requests == 0) {
        printf("Not enough memory\n");
        exit(-1);
    }

    for (int i = 0; i < num_requests; ++i) {
        requests[i] = atoi(argv[i+1]); // atoi: ascii-to-integer
    }

    char *tracks = malloc(DISKSIZE);
    if (tracks == NULL) {
        printf("Out of memory\n");
        exit(-1);
    }

    int startpos = requests[0];
    int path = 0;

    if (algo == FCFS) {
        path = fcfs( tracks, startpos, &requests[1], num_requests-1 );
        printf("(FCFS) %s moved over %d tracks for the request list\n", "Fcfs", path);
    }
    else if (algo == LOOKDOWN) {
        path = lookdown( tracks, startpos, &requests[1], num_requests-1 );
        printf("(LOOKDOWN) %s moved over %d tracks for the request list\n", "Look", path);
    }
    else if (algo == SCAN) {
        path = scan( tracks, startpos, &requests[1], num_requests-1 );
        printf("(SCAN) %s moved over %d tracks for the request list\n", "Look", path);
    }
    else {
        path = look( tracks, startpos, &requests[1], num_requests-1 );
        printf("(LOOK) %s moved over %d tracks for the request list\n", "Look", path);
    }
    // &requests[1] vil si hopper over første, siden gir peker til addressen til neste

    free(tracks);
    free(requests);
    return EXIT_SUCCESS;
}

int fcfs( char *tracks, int pos, int *requests, int num) {
    if (num < 1) return 0;

    // goes through queue iteratively
    int dist = 0;
    for (int i = 0; i < num; ++i) {
        int move = abs(pos - requests[i]);
        dist += move;
        printf("moving from pos %d to pos %d has a move distance of %d\n", pos, requests[i], move);
        pos = requests[i];
    }
    return dist;
}
int lt( const void *l, const void *r) {
    return ( *(int*)l - *(int*)r ); // return difference
}
// const void: ingen datatype, kan holde addresse til hvilken som helst
// -> brukt for generiske funksjoner

int look( char *tracks, int pos, int *requests, int num) {
    printf("Before sort: ");
    for (int i = 0; i < num; ++i) printf("%d ", requests[i]);
    printf("\n");

    qsort( requests, num, sizeof(int), &lt );

    printf("After sort: ");
    for (int i = 0; i < num; ++i) printf("%d ", requests[i]);
    printf("\n");

    int idx = 0;
    while (idx < num && pos > requests[idx]) idx++;

    int dist = 0;
    for (int i = idx; i < num; i++) {
        int move = abs(pos - requests[i]);
        dist += move;
        printf("moving from pos %d to pos %d has a move distance of %d\n", pos, requests[i], move);
        pos = requests[i];
    }
    return dist;
}

int lookdown( char *tracks, int pos, int *requests, int num) {
    printf("Before sort: ");
    for (int i = 0; i < num; ++i) printf("%d ", requests[i]);
    printf("\n");

    qsort( requests, num, sizeof(int), &lt );

    printf("After sort: ");
    for (int i = 0; i < num; ++i) printf("%d ", requests[i]);
    printf("\n");

    int idx = 0;
    while (idx < num && pos > requests[idx]) idx++;

    int dist = 0;
    for (int i = idx-1; i >= 0; i--) {
        int move = abs(pos - requests[i]);
        dist += move;
        printf("moving from pos %d to pos %d has a move distance of %d\n", pos, requests[i], move);
        pos = requests[i];
    }
    return dist;
}

int scan (char *tracks, int pos, int *requests, int num) {
    int *extended = malloc( (num+2)*sizeof(int));
    if (extended == NULL) return look(tracks, pos, requests, num);

    extended[0] = 0;
    extended[1] = DISKSIZE-1;
    memcpy(&extended[2], requests, num * sizeof(int));
    // memcpy: copies size (n) bytes from source (src) to destination (dst)

    int dist = look(tracks, pos, extended, num+2);
    free(extended);
    return dist;
}