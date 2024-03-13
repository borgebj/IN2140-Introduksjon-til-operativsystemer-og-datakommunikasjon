#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>


#define MAXTABLE 1000

typedef struct ArpEntry
{
    uint32_t ip;
    uint8_t  mac[6];
    int      age;
} ArpEntry;

int      latest = 0;
ArpEntry table[MAXTABLE];
int      tableSize;

int validFirstByte( uint8_t byte )
{
    if( ( byte & 8 ) == 0 ) return 1;
    if( ( byte & 4 ) == 0 ) return 1;
    if( ( byte & 2 ) == 0 ) return 1;
    return 0;
}

uint32_t makeRandomIP( )
{
    uint8_t ip[4];
    do {
        ip[0] = lrand48( ) % 256;
    } while( !validFirstByte( ip[0] ) );

    ip[1] = lrand48( ) % 256;
    ip[2] = lrand48( ) % 256;
    ip[3] = lrand48( ) % 256;

    uint32_t ret;
    memcpy( &ret, ip, 4 );
    ret = htonl( ret );
    return ret;
}

void makeRandomMac( uint8_t mac[6] )
{
    for( int i=0; i<6; i++ )
        mac[i] = lrand48( ) % 256;
}

void printEntry( ArpEntry* e )
{
    struct in_addr s;
    memcpy( &s.s_addr, &e->ip, 4 );
    const char* ip = inet_ntoa( s );
    char mac[6*3];
    snprintf( mac, 18, "%02x:%02x:%02x:%02x:%02x:%02x", e->mac[0], e->mac[1], e->mac[2], e->mac[3], e->mac[4], e->mac[5] );
    printf( "%15s  %s  age %d\n", ip, mac, e->age );
}

void printTable( )
{
    for( int i=0; i<tableSize; i++ )
    {
        printEntry( &table[i] );
    }
}

int oldest( const void* li, const void* ri )
{
    struct ArpEntry* l = (struct ArpEntry*)li;
    struct ArpEntry* r = (struct ArpEntry*)ri;
    return ( l->age - r->age );
}

int lt( const void* l, const void* r )
{
    return ( *(const uint32_t*)l - *(const uint32_t*)r );
}

void makeEntry( struct ArpEntry* e )
{
    e->ip = makeRandomIP( );
    makeRandomMac( e->mac );
    e->age = latest++;
}

void generate( int num )
{
    memset( table, 0, MAXTABLE*sizeof(ArpEntry) );

    if( num > MAXTABLE ) num = MAXTABLE;
    tableSize = num;

    for( num=num-1; num>=0; num-- )
    {
        struct ArpEntry* e = &table[num];
        makeEntry( e );
    }

    qsort( table, tableSize, sizeof(ArpEntry), &lt );
}

void replace1( )
{
    qsort( table, tableSize, sizeof(ArpEntry), &oldest );
    makeEntry( &table[0] );
    qsort( table, tableSize, sizeof(ArpEntry), &lt );
}

int main( int argc, char* argv[] )
{
    if( argc < 3 )
    {
        printf("Usage: %s <number> <rep>\n"
               "       where\n"
               "       <number> is a number randomly generated arp entries\n"
               "       <rep> is a number of replacement operations.\n", argv[0] );
        exit( -1 );
    }

    srand48( time(NULL) );

    generate( atoi( argv[1] ) );

    printf( "First table\n" );
    printTable();
    printf( "\n" );

    int rep = atoi( argv[2] );
    for( int i=0; i<rep; i++ )
    {
        replace1( );

        printf( "After %d replacements:\n", i+1 );
        printTable();
        printf( "\n" );
    }

    uint32_t lookfor = table[4].ip;

    struct in_addr in;

    memcpy( &in.s_addr, &lookfor, 4 );
    printf( "Looking up %s\n", inet_ntoa( in ) );
    struct ArpEntry* found = (struct ArpEntry*)bsearch( &lookfor, table, tableSize, sizeof(struct ArpEntry), &lt );
    if( found == NULL )
    {
        printf("Not found\n");
    }
    else
    {
        printf("Found\n");
        printEntry( found );
    }

    return 0;
}