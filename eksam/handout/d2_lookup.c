/* ======================================================================
 * YOU ARE EXPECTED TO MODIFY THIS FILE.
 * ====================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "d2_lookup.h"

/**
 * Creates information required to use server with given name and port
 * @param server_name name to use
 * @param server_port port to use
 * @return a D2Client
 */
D2Client* d2_client_create( const char* server_name, uint16_t server_port )
{
    //TODO: gjør noe med server name og server.port

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
    /* implement this */

    // 1. Create PacketRequest with given id (in host byte order)
    PacketRequest request;
    request.type = (TYPE_REQUEST);
    request.id = id;

    return 0;
}

int d2_recv_response_size( D2Client* client )
{
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

