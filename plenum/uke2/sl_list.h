//
// Created by borge on 24.01.2024.
//
//
// Created by borge on 24.01.2024.
//
#include <stdio.h>

struct SLList;
struct SLListElement;

typedef struct SLList SLList;
typedef struct SLList* SLListPtr;
typedef struct SLListElement* SLListElement;
typedef struct SLListElement* SLListElementPtr;

struct SLListElement;

struct SLList;

void init_list();

int list_enqueue(SLList queue, int pid);
int list_dequeue(SLListPtr queue);