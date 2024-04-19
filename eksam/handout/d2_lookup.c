/* ======================================================================
 * YOU ARE EXPECTED TO MODIFY THIS FILE.
 * ====================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "d2_lookup.h"

#define REQUEST_SIZE 64

//TODO: debugging - remove
//void printbits(void *n, int size) {
//    char *num = (char *)n;
//    int i, j;
//
//    for (i = size-1; i >= 0; i--) { // itererer gjennom bytes
//        for (j = 7; j >= 0; j--) {
//            printf("%c", (num[i] & (1 << j)) ? '1' : '0');
//        }
//        printf(" ");
//    }
//    printf("\n");
//}

/**
 * Creates information required to use server with given name and port
 * @param server_name name to use
 * @param server_port port to use
 * @return a D2Client
 */
D2Client* d2_client_create( const char* server_name, uint16_t server_port )
{
    D2Client *client = malloc(sizeof(D2Client));
    if (client != NULL) {
        D1Peer *peer = d1_create_client();
        d1_get_peer_info(peer, server_name, server_port);
        client->peer = peer;
        return client;
    }
    return NULL;
}

/**
 * Deletes information and state required to communicate with server
 * @param client client to delete
 * @return NULL
 */
D2Client* d2_client_delete( D2Client* client )
{
    d1_delete(client->peer);
    free(client);
    return NULL;
}

int d2_send_request( D2Client* client, uint32_t id )
{
    // creates PacketRequest, sets and converts its value-fields
    PacketRequest request;
    request.type = htons(TYPE_REQUEST);
    uint16_t empty = 0;
    request.id = htonl(id);

    // create the packet to be sent over network
    char buffer[REQUEST_SIZE];
    memcpy(buffer, &request.type, sizeof(uint16_t));
    memcpy(buffer + sizeof(uint16_t), &empty, sizeof(uint16_t));
    memcpy(buffer + sizeof(uint32_t), &request.id, sizeof(uint32_t));

    // TODO: remove, debug
    uint16_t pack_type = ntohs(*((uint16_t*)buffer));
    uint16_t pack_empty = *((uint16_t*)(buffer + sizeof(uint16_t)));
    uint32_t pack_id = ntohl(*((uint32_t*)(buffer + 2 * sizeof(uint16_t))));
    printf("pack type:\t"); printbits(&pack_type, sizeof(uint16_t));
    printf("pack empty:\t"); printbits(&pack_empty, sizeof(uint16_t));
    printf("pack id:\t"); printbits(&pack_id, sizeof(uint32_t));

    int bytes_sent = d1_send_data(client->peer, buffer, REQUEST_SIZE);
    if (bytes_sent < 0) return 0;
    return 1;
}

int d2_recv_response_size( D2Client* client )
{
    int ret;
    char buffer[MAX_PACKETSIZE];

    // receives the data
    ret = d1_recv_data( client->peer, buffer, 1000 );
    if( ret < 0 )
    {
        d1_delete( client->peer );
        return -1;
    }

    PacketHeader *header = (PacketHeader *)buffer;
    uint16_t packet_type = ntohs(header->type);

    printf("Type:\t"); printbits(&packet_type, sizeof(uint16_t));
    if (packet_type & TYPE_RESPONSE_SIZE) {
        printf("Received response size!\n");
        PacketResponseSize *responseSize = (PacketResponseSize *)buffer;
        uint16_t packet_size = ntohs(responseSize->size);
        printf("Size:\t(%d)\t", packet_size); printbits(&packet_size, sizeof(uint16_t));
    }
    else {
        printf("Didnt receive response size :(\n");
    }

    exit(-1);
    /* implement this */
    return 0;
}

int d2_recv_response( D2Client* client, char* buffer, size_t sz )
{
    /* implement this */
    return 0;
}

LocalTreeStore* d2_alloc_local_tree( int num_nodes )
{
    /* implement this */
    return NULL;
}

void  d2_free_local_tree( LocalTreeStore* nodes )
{
    /* implement this */
}

int d2_add_to_local_tree( LocalTreeStore* nodes_out, int node_idx, char* buffer, int buflen )
{
    /* implement this */
    return 0;
}

void d2_print_tree( LocalTreeStore* nodes_out )
{
    /* implement this */
}

