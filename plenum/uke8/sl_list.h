#ifndef IN2140_SLLIST
#define IN2140_SLLIST

struct List;
struct Element;

struct Element
{
    int start;
    int size;
    struct Element* next;
};

struct List
{
    struct Element* first;
};

typedef struct Element Element;
typedef struct List    List;

Element* element_new( );
Element* element_new_init( int start, int size );
void     element_delete( Element* elem );

void list_init( List* list );
void list_delete( List* list );
void list_append( List* list, Element* elem );

void list_insert( List* list, Element* elem );
void list_merge( Element* base, Element* add );
void list_insert_and_merge( List* list, Element* elem );

Element* find_first( List* list, int size );

void list_print( List* list );
void list_draw( List* list, int limit );

#endif /* IN2140_SLLIST */

