#include <stdio.h>
#include "sl_list.h"

void test_append( )
{
    List free_mem;

    list_init( &free_mem );

    printf("In line %d: ", __LINE__ );
    list_print( &free_mem );

    list_append( &free_mem, element_new_init( 128, 64 ) );
    printf("In line %d: ", __LINE__ ); list_print( &free_mem );

    list_append( &free_mem, element_new_init( 0, 64 ) );
    printf("In line %d: ", __LINE__ ); list_print( &free_mem );

    list_append( &free_mem, element_new_init( 192, 64 ) );
    printf("In line %d: ", __LINE__ ); list_print( &free_mem );

    list_append( &free_mem, element_new_init( 256, 64 ) );
    printf("In line %d: ", __LINE__ ); list_print( &free_mem );

    list_append( &free_mem, element_new_init( 64, 64 ) );
    printf("In line %d: ", __LINE__ ); list_print( &free_mem );

    list_delete( &free_mem );

    printf("In line %d: ", __LINE__ ); list_print( &free_mem );
}

void test_insert( )
{
    List free_mem;

    list_init( &free_mem );

    printf("In line %d: ", __LINE__ );
    list_print( &free_mem );

    list_insert( &free_mem, element_new_init( 128, 64 ) );
    printf("In line %d: ", __LINE__ ); list_print( &free_mem );

    list_insert( &free_mem, element_new_init( 0, 64 ) );
    printf("In line %d: ", __LINE__ ); list_print( &free_mem );

    list_insert( &free_mem, element_new_init( 192, 64 ) );
    printf("In line %d: ", __LINE__ ); list_print( &free_mem );

    list_insert( &free_mem, element_new_init( 256, 64 ) );
    printf("In line %d: ", __LINE__ ); list_print( &free_mem );

    list_insert( &free_mem, element_new_init( 64, 64 ) );
    printf("In line %d: ", __LINE__ ); list_print( &free_mem );

    list_delete( &free_mem );

    printf("In line %d: ", __LINE__ ); list_print( &free_mem );
}

void test_insert_and_merge( )
{
    List free_mem;

    list_init( &free_mem );

    printf("In line %d: ", __LINE__ );
    list_print( &free_mem );

    list_insert_and_merge( &free_mem, element_new_init( 128, 64 ) );
    printf("In line %d: ", __LINE__ ); list_print( &free_mem );

    list_insert_and_merge( &free_mem, element_new_init( 0, 64 ) );
    printf("In line %d: ", __LINE__ ); list_print( &free_mem );

    list_insert_and_merge( &free_mem, element_new_init( 192, 64 ) );
    printf("In line %d: ", __LINE__ ); list_print( &free_mem );

    list_insert_and_merge( &free_mem, element_new_init( 256, 64 ) );
    printf("In line %d: ", __LINE__ ); list_print( &free_mem );

    list_insert_and_merge( &free_mem, element_new_init( 64, 64 ) );
    printf("In line %d: ", __LINE__ ); list_print( &free_mem );

    list_delete( &free_mem );

    printf("In line %d: ", __LINE__ ); list_print( &free_mem );
}

int main( )
{
    printf("Choose one option:\n"
           "    0 - append testing\n"
           "    1 - insert testing\n"
           "    2 - insert and merge testing\n" );
    int c = getchar();
    switch(c)
    {
    case '0' :
        test_append( );
        break;
    case '1' :
        test_insert( );
        break;
    case '2' :
        test_insert_and_merge( );
        break;
    default :
        printf("You can only choose 0, 1 or 2 - doing nothing.\n");
        break;
    }
}

