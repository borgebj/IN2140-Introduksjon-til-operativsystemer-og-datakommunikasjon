#include "af_map.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <sys/socket.h> // for basic socket functions
#include <sys/errno.h> // for the global variable errno
#include <ifaddrs.h> // for getifaddrs and freeifaddrs
#include <netdb.h> // for getaddrinfo and freeaddrinfo and gai_strerror
#include <string.h> // for strerror

#ifdef __linux__
#define HAS_AF_PACKET 1
#endif


void list_all_network_interfaces( )
{
    struct if_nameindex* if_head = if_nameindex();

    if( if_head == NULL )
    {
        printf("The system has no network interfaces at all\n");
        return;
    }

    struct if_nameindex* if_it = if_head;

    while( if_it->if_index != 0 || if_it->if_name != NULL )
    {
        if( if_it->if_name )
        {
            printf( "Interface with name %s\n", if_it->if_name );
        }
        else
        {
            printf( "Unknamed interface with index %d\n", if_it->if_index );
        }

        if_it++;
    }

    if_freenameindex( if_head );
}

void list_all_network_interfaces_with_address( )
{
    struct ifaddrs* if_head;

    int err = getifaddrs( &if_head );
    if( err < 0 )
    {
        printf( "The system has no network interfaces that are active: %s\n", strerror(errno) );
        return;
    }

    struct ifaddrs* if_it = if_head;

    while( if_it != NULL )
    {
        if( if_it->ifa_addr )
        {
            switch( if_it->ifa_addr->sa_family )
            {
                case AF_INET  :
                case AF_INET6 :
                case AF_UNIX  :
                    break;
                default :
                    if_it = if_it->ifa_next;
                    continue;
            }

            if( if_it->ifa_name )
                printf( "Interface: name %s ", if_it->ifa_name );
            else
                printf( "Unnamed interface " );

            struct sockaddr* if_addr = if_it->ifa_addr;
            struct sockaddr* if_netmask = if_it->ifa_netmask;

            switch( if_it->ifa_addr->sa_family )
            {
                case AF_INET  : printf( "AF_INET (IPv4 interface) " ); break;
                case AF_INET6 : printf( "AF_INET6 (IPv6 interface) " ); break;
                case AF_UNIX  : printf( "AF_UNIX : an interface for IPC using packets " ); break;
            }

            if( if_it->ifa_addr->sa_family == AF_INET || if_it->ifa_addr->sa_family == AF_INET6 )
            {
                // struct sockaddr_in* addr = (struct sockaddr_in*)if_addr;
                // struct sockaddr_in* mask = (struct sockaddr_in*)if_netmask;

                socklen_t addrlen = 0;

                if( if_it->ifa_addr->sa_family == AF_INET ) addrlen = sizeof(struct sockaddr_in);
                else if( if_it->ifa_addr->sa_family == AF_INET6 ) addrlen = sizeof(struct sockaddr_in6);

                char name[50];
                char service[50];
                int flags = NI_NUMERICHOST |   // provide the host address information as numbers, don't look up name
                            NI_NUMERICSERV;    // provide the service name as numbers, don't look up name

                int err = getnameinfo( if_addr, addrlen,
                                       name, 50,
                                       service, 50,
                                       flags );
                if( err != 0 )
                {
                    printf( " no name/service info (%s)\n", gai_strerror( err ) );
                }
                else
                {
                    printf( "addr %s srv %s\n", name, service );
                }
            }
            else
            {
                printf( "\n" );
            }
        }

        if_it = if_it->ifa_next;
    }

    if_it = if_head;

    while( if_it != NULL )
    {
        if( if_it->ifa_addr )
        {
            switch( if_it->ifa_addr->sa_family )
            {
                case AF_INET  :
                case AF_INET6 :
                case AF_UNIX  :
                    if_it = if_it->ifa_next;
                    continue;
                default :
                    printf( "%s ", af_map(if_it->ifa_addr->sa_family) );
                    break;
            }

            if( if_it->ifa_name )
                printf( "Interface: name %s ", if_it->ifa_name );
            else
                printf( "Unnamed interface " );

            struct sockaddr* if_addr = if_it->ifa_addr;
            struct sockaddr* if_netmask = if_it->ifa_netmask;

            printf( "%s\n", af_map(if_it->ifa_addr->sa_family) );
        }

        if_it = if_it->ifa_next;
    }

    freeifaddrs( if_head );
}

int main( int argc, char* argv[] )
{
    printf("Printing all interfaces:\n");
    list_all_network_interfaces( );
    printf("\nPrinting all the interfaces that have an address:\n");
    list_all_network_interfaces_with_address( );
}