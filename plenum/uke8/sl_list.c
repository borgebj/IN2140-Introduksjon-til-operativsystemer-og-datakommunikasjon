#include <stdio.h>
#include <stdlib.h>
#include "sl_list.h"

Element* element_new( )
{
    /* allocate an element and initialize empty */
    Element* elem = malloc(sizeof(Element));
    elem -> start = 0;
    elem -> size = 0;
    elem -> next = NULL;
    return elem;
}

Element* element_new_init( int start, int size )
{
    /* allocate an element and initialize with parameters */
    Element* elem = malloc(sizeof(Element));
    elem -> start = start;
    elem -> size = size;
    elem -> next = NULL;
    return elem;}

void element_delete( Element* elem )
{
    /* deallocate an element */
    if (elem != NULL) free(elem);
}

void list_init( List* list )
{
    /* initialize to an empty list */
    list -> first = NULL;
}

void list_delete( List* list )
{
    /* deallocate all elements currently in the list */
    while (list -> first != NULL) {
        Element* elem = list -> first;
        list -> first = elem -> next;
        element_delete(elem);
    }
}

void list_append( List* list, Element* elem )
{
    /* append the given element at the end of the list */
    // bruker pointer-iterator til slutten
    Element* iterator = list -> first;

    if (iterator == NULL) {
        // handles empty lists
        list -> first = elem;
        return;
    }

    while (iterator->next != NULL) {
        iterator = iterator -> next;
    }
    // if next is NULL = end of list = append
    iterator -> next = elem;
}

void list_insert( List* list, Element* elem )
{
    /* Assume that the list is sorted on entry.
     * Search for the last element whose start value is below elem->start and
     * insert after that element.
     * Do not check if the old and new ranges overlap.
     */

    if (list -> first == NULL) {
        list -> first = elem;
        return;
    }

    Element* iterator = list -> first;

    // special case: first is bigger
    if (iterator -> start > elem -> start) {
        elem -> next = iterator;
        list -> first = elem;
        return;
    }

    // iterate and compare
    while (iterator -> next != NULL) {
        if (iterator -> next -> start < elem -> start) {
            iterator = iterator -> next;
            continue;
        }
        elem -> next = iterator -> next;
        iterator -> next = elem;
        return;
    }
}

void list_compress( Element* base )
{
    /* support function for list_insert_and_merge */
    if (base -> next == NULL) return;

    // if exists: merge
    if (base -> start + base -> size == base -> next -> start) {
        base -> size = base -> size + base -> next -> size;

        Element* elem = base -> next;
        base -> next = elem -> next;

        element_delete(elem);
    }
}

void list_insert_and_merge( List* list, Element* elem )
{
    /* Do the same as list_insert, but whenever e->start + e->size == e->next->start,
     * merge those two elements into a single one.
     */

    if (list -> first == NULL) {
        list -> first = elem;
        return;
    }

    Element* iterator = list->first;
    if (iterator -> start > elem -> start) {
        elem -> next = iterator;
        list -> first = elem;
        list_compress(elem);
        return;
    }

    while (iterator -> next != NULL) {
        if (iterator -> next -> start < elem -> start) {
            iterator = iterator -> next;
            continue;
        }
        elem -> next = iterator -> next;
        list_compress(elem);
        iterator -> next = elem;
        list_compress(iterator);
        return;
    }
    iterator -> next = elem;
    list_compress(iterator);
}

Element* find_first( List* list, int size )
{
    /* Find the first Element e whose size is larger or equal size.
     * If sizes are equal, remove e from the list and return it.
     * If e->size > size, create a new element with e->start and size
     * as values, and change e->start and e->size changed to reflect
     * that size units have been removed at the start of block e.
     */

    // case: empty list
    if (list -> first == NULL) return NULL;

    Element* iterator = list -> first;

    // case: første element
    if (iterator -> size >= size)
    {
        Element* elem = element_new_init(iterator->start, size);
        iterator -> start = iterator -> start + size;
        iterator -> size = iterator -> size - size;

        if (iterator->size == 0)
        {
            list -> first = iterator -> next;
            element_delete(iterator);
        }
        return elem;
    }

    while (iterator -> next != NULL) {
        if (iterator -> next -> size >= size) {
            Element* elem = element_new_init(iterator->next->start, size);
            iterator -> next -> start = iterator -> start + size;
            iterator -> next -> size = iterator -> size - size;

            if (iterator->next -> size == 0) {
                Element* takeout = iterator->next;
                iterator->next = iterator->next->next;
                element_delete(takeout);
            }
            return elem;
        }
        iterator = iterator -> next;
    }
    return NULL;
}

void list_print( List* list )
{
    printf("List:\n");

    if( list->first == NULL )
    {
        printf("    EMPTY\n");
        return;
    }

    Element* iterator = list->first;
    while( iterator )
    {
        printf("    from: %d size: %d, next: %d\n", iterator->start, iterator->size, iterator->start + iterator->size );
        iterator = iterator->next;
    }
}

void list_draw( List* list, int limit )
{
    int i = 0;

    if( list->first == NULL )
    {
        for( i=0; i<limit; i++ ) printf("X");
        printf("\n");
        return;
    }

    Element* iterator = list->first;
    while( iterator != NULL )
    {
        for(    ; i<iterator->start; i++ ) printf("X");
        for(    ; i<iterator->start + iterator->size; i++ ) printf(".");
        iterator = iterator->next;
    }
    for(    ; i<limit; i++ ) printf("X");
    printf("\n");
}

