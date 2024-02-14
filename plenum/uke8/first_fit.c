#include <stdio.h>
#include "sl_list.h"

List free_mem;

void init( int size )
{
    list_init( &free_mem );

    /// add a single block with all the "free" elements
    // hele minneområde
    list_insert_and_merge( &free_mem, element_new_init( 0, size ) );
}

void cleanup( )
{
    /// delete all elements in the list
    list_delete( &free_mem );
}

Element* first_fit_alloc( int size )
{
    Element* retval;

    printf("\nAllocating %d: ", size );
    getchar(); // waiting for a keypress

    /// try to allocate a free block of size elements and return it
    retval = find_first( &free_mem, size );
    if( retval ) printf( "Success from %d size %d\n", retval->start, retval->size );
    else         printf( "Failed\n" );
    list_draw( &free_mem, 16 ); // draw free and used elements

    return retval;
}

void first_fit_free( Element* elem )
{
    printf("\nDeallocating from %d size %d\n", elem->start, elem->size );
    getchar();

    /// return the elements given by elem to the free space list
    list_insert_and_merge( &free_mem, elem );
    list_draw( &free_mem, 16 );
}

int main( )
{
    Element* elem[20];

    printf("Initializing, empty memory\n");
    init( 16 );
    list_draw( &free_mem, 16 );

    elem[0] = first_fit_alloc( 1 );
    elem[1] = first_fit_alloc( 5 );
    elem[2] = first_fit_alloc( 5 );
    elem[3] = first_fit_alloc( 6 );
    elem[4] = first_fit_alloc( 5 );
    elem[5] = first_fit_alloc( 1 );

    first_fit_free( elem[1] );
    first_fit_free( elem[4] );

    elem[6] = first_fit_alloc( 1 );
    elem[7] = first_fit_alloc( 1 );
    elem[8] = first_fit_alloc( 1 );
    elem[9] = first_fit_alloc( 1 );

    first_fit_free( elem[6] );

    elem[10] = first_fit_alloc( 2 );

    elem[11] = first_fit_alloc( 2 );

    first_fit_free( elem[8] );

    first_fit_free( elem[0] );

    printf("\nCleaning up\n");
    cleanup( );
}

