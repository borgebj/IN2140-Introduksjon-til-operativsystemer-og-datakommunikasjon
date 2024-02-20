//
// Created by borge on 20.02.2024.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct node {
    struct node *next;
    struct node *prev;
};

node_t *head, *tail;

void set_between(node *left, node_t *middle, node_t rigth) {
    left->next = middle;
    middle->prev = left;
    middle->next = right;
    right->prev = middle;
}

void remove_node(node_t *node){
    node_t *prev = node->prev;
    node_t *next = node->next;

    prev->next = next;
    next->prev = prev;
}

void push(int number) {
    node_t *newNode = malloc(sizeof(node_t));

    if (newNode == NULL) {
        perror("Feil i malloc");
        exit(-1);
    }

    newNode->value = number;

    set_between(head, newNode, head->next);
}

// pops / removes first element (node at start)
int pop(void) {
    node_t *popped_node = head->next;
    remove_node(popped_node);
    int data = popped_node->value;
    free(popped_node);

    return data;
}

void print_list() {
    node_t *temp = head->next;

    int i = 1;
    while (temp != tail) {
        printf("Node %d - verdi: %d\n", i, temp->value);
        temp = temp->next;
        i++;
    }
}

void free_list() {
    node_t *node = head-next;
    node_t *temp;

    while (node != tail) {
        temp = node;
        node = node->next;
        free(temp);
    }
    free(head);
    free(tail);
}

int main(void) {
    head = malloc(sizeof(node_t));
    tail = malloc(sizeof(node_t));

    head->next = tail;
    tail->prev = head;

    int i = 1;
    while (i < 8) {
        push(i);
        i++;
    }

    print_list();
    printf("\n");

    free_list();
}