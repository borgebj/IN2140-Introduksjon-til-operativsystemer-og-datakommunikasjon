#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>


#define MAXTABLE 1000

typedef struct NetEntry
{
    uint32_t ip;
    uint32_t mask;
    int      interface;
} NetEntry;

NetEntry table[1000];
int tableSize = 1000;

int validFirstByte( uint8_t byte )
{
    if( ( byte & 0x80 ) == 0 ) return 1;
    if( ( byte & 0x40 ) == 0 ) return 1;
    if( ( byte & 0x20 ) == 0 ) return 1;
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

    uint32_t ret = ( ip[0] << 24 )
                   | ( ip[1] << 16 )
                   | ( ip[2] <<  8 )
                   | ( ip[3] <<  0 );

    return ret;
}

uint32_t makeMask( int len )
{
    uint8_t ip[4];
    ip[0] = 255;
    ip[1] = 255;
    ip[2] = 255;
    ip[3] = 255;

    if( len < 16 )
    {
        switch( len )
        {
            case  8 : ip[1] = 0;   break;
            case  9 : ip[1] = 128; break;
            case 10 : ip[1] = 192; break;
            case 11 : ip[1] = 224; break;
            case 12 : ip[1] = 240; break;
            case 13 : ip[1] = 248; break;
            case 14 : ip[1] = 252; break;
            case 15 : ip[1] = 254; break;
        }
        ip[2] = 0;
        ip[3] = 0;
    }
    else if( len < 24 )
    {
        switch( len )
        {
            case 16 : ip[2] = 0;   break;
            case 17 : ip[2] = 128; break;
            case 18 : ip[2] = 192; break;
            case 19 : ip[2] = 224; break;
            case 20 : ip[2] = 240; break;
            case 21 : ip[2] = 248; break;
            case 22 : ip[2] = 252; break;
            case 23 : ip[2] = 254; break;
        }
        ip[3] = 0;
    }
    else
    {
        switch( len )
        {
            case 24 : ip[3] = 0;   break;
            case 25 : ip[3] = 128; break;
            case 26 : ip[3] = 192; break;
            case 27 : ip[3] = 224; break;
            case 28 : ip[3] = 240; break;
            case 29 : ip[3] = 248; break;
            case 30 : ip[3] = 252; break;
        }
    }

    uint32_t ret = ( ip[0] << 24 )
                   | ( ip[1] << 16 )
                   | ( ip[2] <<  8 )
                   | ( ip[3] <<  0 );

    return ret;
}

int makeEntry( struct NetEntry* e )
{
    int ct = 0;
    uint32_t ip = makeRandomIP( );

    int masklen = 8 + lrand48() % 23; // max is 30

    printf("Masklens: ");
    while( masklen <= 30 )
    {
        printf("%d ", masklen);

        e->ip        = ip;
        e->mask      = makeMask( masklen );
        e->interface = lrand48() % 4;
        e--;
        ct++;

        int newmasklen = 16 + lrand48() % 14;
        if( newmasklen > masklen )
            masklen = newmasklen;
        else
            break;
    }

    printf( "\n" );
    return ct;
}

void printEntry( NetEntry* e )
{
#if 0
    printf( "%08x %08x\n", e->ip, e->mask );
#else
    if( e == NULL )
    {
        fprintf(stderr, "NetEntry %p is invalid\n", (void*)e);
    }

    e->ip = e->ip & e->mask;

    struct in_addr s;
    s.s_addr = htonl(e->ip);
    char ip[18];
    strncpy( ip, inet_ntoa( s ), 18 );

    s.s_addr = htonl(e->mask);
    char mask[18];
    strncpy( mask, inet_ntoa( s ), 18 );

    char class = '-';
    if     ( ( e->ip & ( (uint32_t)0x80 << 24 ) ) == 0 ) class = 'A';
    else if( ( e->ip & ( (uint32_t)0x40 << 24 ) ) == 0 ) class = 'B';
    else if( ( e->ip & ( (uint32_t)0x20 << 24 ) ) == 0 ) class = 'C';

    printf( "%c %15s  %15s  if %d\n", class, ip, mask, e->interface );
#endif
}

void printTable( )
{
    printf("Printing %d elements\n", tableSize);
    for( int i=0; i<tableSize; i++ )
    {
        printEntry( &table[i] );
    }
}

int lt( const void* li, const void* ri )
{
    const uint32_t* l = (const uint32_t*)li;
    const uint32_t* r = (const uint32_t*)ri;
    int64_t cmp = ( (int64_t)*l - (int64_t)*r );
    int ret = ( cmp == 0 ) ? 0
                           : ( cmp < 0 ) ? -1 : 1;
    return ret;
}

void generate( int num )
{
    memset( table, 0, MAXTABLE*sizeof(NetEntry) );

    if( num > MAXTABLE ) num = MAXTABLE;
    tableSize = num;

    int howmany;
    for( num=num-1; num>=0; num -= howmany )
    {
        struct NetEntry* e = &table[num];
        howmany = makeEntry( e );
    }

    printf("Sorting %d elements\n", tableSize);
    qsort( table, tableSize, sizeof(NetEntry), &lt );
}

uint32_t createSearch( )
{
    struct in_addr s;
    char ip[18];

    int idx = lrand48() % tableSize;

    NetEntry* e = &table[idx];

    // s.s_addr = htonl(e->ip);
    // strncpy( ip, inet_ntoa( s ), 18 );
    // printf( "Pick an address in %s ", ip );
    // s.s_addr = htonl(e->mask);
    // strncpy( ip, inet_ntoa( s ), 18 );
    // printf( "%s: ", ip );

    uint32_t prefix = e->ip & e->mask;
    // s.s_addr = htonl(prefix);
    // strncpy( ip, inet_ntoa( s ), 18 );
    // printf( "prefix %s ", ip );

    uint32_t address = lrand48();
    // s.s_addr = htonl(address);
    // strncpy( ip, inet_ntoa( s ), 18 );
    // printf( "rnd.pick %s ", ip );
    address = ( address & ~e->mask ) | prefix;
    s.s_addr = htonl(address);
    strncpy( ip, inet_ntoa( s ), 18 );
    printf( "search %s\n", ip );

    return address;
}

int maskedCompare( const void* keyPtr, const void* entryPtr )
{
    uint32_t key = *(const uint32_t*)keyPtr;
    const NetEntry* entry = (const NetEntry*)entryPtr;

    uint32_t l = key & entry->mask;
    uint32_t r = entry->ip & entry->mask;

#if 1
    struct in_addr s;
    char ip[18];

    s.s_addr = htonl(l);
    strncpy( ip, inet_ntoa( s ), 18 );
    printf("Comparing %s ", ip);

    s.s_addr = htonl(r);
    strncpy( ip, inet_ntoa( s ), 18 );
    printf("and %s ", ip);
#endif

    int64_t cmp = ( (int64_t)l - (int64_t)r );
    int ret = ( cmp == 0 ) ? 0
                           : ( cmp < 0 ) ? -1 : 1;
#if 1
    printf("-> %d\n", ret );
#endif
    return ret;
}

void doSearch( uint32_t addr )
{
    NetEntry* result = (NetEntry*)bsearch( &addr, table, tableSize, sizeof(NetEntry), &maskedCompare );

    if( result == 0 )
    {
        printf("Failed to find this address\n" );
        return;
    }

    printEntry( result );

    while( maskedCompare( &addr, result-1 ) == 0 )
    {
        result = result - 1;
        printEntry( result );
    }

}

int main( int argc, char* argv[] )
{
    if( argc < 2 )
    {
        printf("Usage: %s <number>\n"
               "       where\n"
               "       <number> is a number randomly generated network entries\n", argv[0] );
        exit( -1 );
    }

    srand48( time(NULL) );

    generate( atoi( argv[1] ) );

    printf( "First table\n" );
    printTable();
    printf( "\n" );

    uint32_t search = createSearch( );

    doSearch( search );

    return 0;
}