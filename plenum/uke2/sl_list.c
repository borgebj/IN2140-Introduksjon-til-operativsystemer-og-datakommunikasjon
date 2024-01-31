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

struct SLListElement {
    int pid;
    SLListElementPtr next;
};

struct SLList {
    SLListElementPtr first;
};

struct SLListElement elements[1000];

SLListElementPtr unused_elements = NULL;

void init_list() {
    for (int i=0; i < 1000; ++i) {
        elements[i].pid = -1;
        elements[i].next = unused_elements;
        unused_elements = &elements[i];
    }
}

int list_enqueue(SLList queue, int pid) {
    SLListElementPtr q = unused_elements;
    if (q == NULL) {
        printf("There are no more data structures for tasks\n");
        return 0;
    }

    q->pid = pid;
    q->next = NULL;

    if (queue->first == NULL) {
        queue->first = q;
        return pid;
    }

    SLListElementPtr* iterator = list->first;
    while (iterator->next != NULL) {
        iterator = iterator->next;
    }
    iterator->next = q;
    return pid;
}

int list_dequeue(SLListPtr queue) {
    if (queue == NULL) return -1;
    if (queue->first == NULL) return -1;

    SLListElementPtr ptr = queue->first;
    queue->first = ptr->next;

    int pid = ptr->pid;
    ptr->next = unused_elements;
    unused_elements = ptr;

    return pid;
}
