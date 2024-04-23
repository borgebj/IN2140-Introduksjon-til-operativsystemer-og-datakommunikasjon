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
void printbits(void *n, int size) {
    char *num = (char *)n;
    int i, j;

    for (i = size-1; i >= 0; i--) { // itererer gjennom bytes
        for (j = 7; j >= 0; j--) {
            printf("%c", (num[i] & (1 << j)) ? '1' : '0');
        }
        printf(" ");
    }
    printf("\n");
}

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
        if ( !peer ) {
            printf( "Failed to create D1 client.\n" );
            return NULL;
        }

        int ret = d1_get_peer_info(peer, server_name, server_port);
        if( ret == 0 ) {
            printf( "Failed to resolve the name for %s:%d\n", server_name, server_port);
            d1_delete(peer);
            return NULL;
        }
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

/**
 * Sends a D2 requests through the given client
 * Contains header-type and id.
 * @param client containing info about peer
 * @param id id to send through client in header
 * @return > 0 if success, <= 0 if error
 */
int d2_send_request( D2Client* client, uint32_t id )
{
    if (id <= 1000) {
        printf("ID must be >1000 ...\n");
        return -1;
    }

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

//    // TODO: remove, debug
    uint16_t pack_type = ntohs(*((uint16_t*)buffer));
    uint16_t pack_empty = *((uint16_t*)(buffer + sizeof(uint16_t)));
    uint32_t pack_id = ntohl(*((uint32_t*)(buffer + 2 * sizeof(uint16_t))));
    printf("\n[ Before sending request, packet: ]\n");
    printf("pack type:\t"); printbits(&pack_type, sizeof(uint16_t));
    printf("pack empty:\t"); printbits(&pack_empty, sizeof(uint16_t));
    printf("pack id:\t"); printbits(&pack_id, sizeof(uint32_t));
    printf("[ Before sending request, packet: ]\n\n");

    // sends the packet in the buffer
    int bytes_sent = d1_send_data(client->peer, buffer, REQUEST_SIZE);
    if (bytes_sent < 0) return 0;
    return 1;
}

int d2_recv_response_size( D2Client* client )
{
    // buffer for receiving response
    char buffer[MAX_PACKETSIZE];

    // receives the PacketResponseSize inside the buffer
    int ret = d1_recv_data(client->peer, buffer, 1024);
    if( ret < 0 ) {
        printf("Failed to receive data packet ...\n");
        return -1;
    }

    // parse type from PacketHeader-cast
    PacketHeader *header = (PacketHeader *)buffer;
    uint16_t packet_type = ntohs(header->type);

    // checks for right packet-type
    if (packet_type & TYPE_RESPONSE_SIZE) {

        // cast to responseSize to retrieve the size
        PacketResponseSize *responseSize = (PacketResponseSize *)buffer;
        uint16_t packet_size = ntohs(responseSize->size);

        printf("%d: Received a ResponseSize of %d\n", getpid(), packet_size);
        return packet_size;
    }
    else {
        printf("Failed to receive response size ...\n");
        return -1;
    }
}

int d2_recv_response( D2Client* client, char* buffer, size_t sz )
{
    // receives the PacketResponse inside the given buffer
    int bytes_received = d1_recv_data(client->peer, buffer, sz);
    if( bytes_received < 0 ) {
        printf("Failed to receive data packet ...\n");
        return -1;
    }

    // parse type from PacketHeader-cast
    PacketHeader *header = (PacketHeader *)buffer;
    uint16_t packet_type = ntohs(header->type);

    // check correct type
    if (packet_type == TYPE_RESPONSE || packet_type == TYPE_LAST_RESPONSE) {

        // cast to Response to retrieve data
        PacketResponse *response = (PacketResponse *)buffer;
        uint16_t payload_size = ntohs(response->payload_size);

        // check size retrieved
        if (payload_size > 0 && bytes_received == payload_size) {

            size_t bytes_seen = 0;
            size_t remaining_bytes = payload_size - sizeof(PacketResponse);
            uint32_t *ptr = (uint32_t *)(buffer + sizeof(PacketResponse));

            // Parse each node from buffer based on how many bytes
            while (remaining_bytes >= sizeof(uint32_t) * 3) {
                NetNode node;

                // Retrieve and convert byte order from payload buffer
                node.id = ntohl(*ptr++);
                node.value = ntohl(*ptr++);
                node.num_children = ntohl(*ptr++);

                // Read in children-ids
                uint32_t num_children = node.num_children;
                for (size_t i = 0; i < num_children && i < 5; ++i) {
                    node.child_id[i] = ntohl(*ptr++);
                }

//                // Print the parsed NetNode structure
//                printf("\nNetNode id: %u\n", node.id);
//                printf("NetNode value: %u\n", node.value);
//                printf("NetNode num_children: %u\n", node.num_children);
//                printf("NetNode child_id: ");
//                for (size_t i = 0; i < num_children && i < 5; ++i) {
//                    printf(" '%u' ", node.child_id[i]);
//                }
//                printf("\n");

                bytes_seen += sizeof(NetNode);
                remaining_bytes -= sizeof(uint32_t) * (3 + num_children);
            }

            // in case of success: fill new buffer and returns bytes received
            return bytes_received;
        }
    }

    // fail-case: returns 0
    return 0;
}

LocalTreeStore* d2_alloc_local_tree( int num_nodes )
{
    LocalTreeStore *tree = malloc(sizeof(LocalTreeStore));
    if (tree != NULL) {
        tree->number_of_nodes = num_nodes;

        // allocate space for NetNodes
        printf("%d: Allocating space for %d nodes\n", getpid(), num_nodes);
        tree->nodes = malloc(num_nodes * sizeof(NetNode));
        if (tree->nodes == NULL) {
            free(tree);
            return NULL;
        }
        return tree;
    }
    return NULL;
}

void  d2_free_local_tree( LocalTreeStore* nodes )
{
    // Deletes tree by freeing allocated space
    if (nodes != NULL) {
        if (nodes->nodes != NULL) {
            free(nodes->nodes);
        }
        free(nodes);
    }
}

/**
 * Adds nodes in buffer to the local tree
 * @param nodes tree to contain nodes
 * @param node_idx
 * @param buffer payload containing bytes with node-info
 * @param buflen the length of the payload
 * @return
 */
int d2_add_to_local_tree( LocalTreeStore* nodes, int node_idx, char* buffer, int buflen )
{
    printf("\n\n==============================\n");
    printf("[ Add to tree ]\n");
    printf("Index: %d\n", node_idx);
    printf("max nodes: %d\n", nodes->number_of_nodes);
    printf("------------------------------");

    // Pointer to traverse the buffer
    uint32_t *ptr = (uint32_t *)buffer;

    // Iterate through the buffer until no more bytes are left
    while ((char*)ptr < buffer + buflen) {
        NetNode node;

        // Convert byte order using ntohl for each field
        node.id = ntohl(*ptr++);
        node.value = ntohl(*ptr++);
        node.num_children = ntohl(*ptr++);

        uint32_t num_children = node.num_children;
        for (size_t j = 0; j < num_children && j < 5; ++j) {
            node.child_id[j] = ntohl(*ptr++);
        }



        //TODO: remove
        printf("\nIndex:\t\t%d\n", node_idx);
        printf("NetNode id:\t\t%u\n", node.id);
        printf("NetNode value:\t\t%u\n", node.value);
        printf("NetNode num_children:\t%u\n", node.num_children);
        printf("NetNode child_id:\t");
        for (size_t j = 0; j < node.num_children && j < 5; ++j) {
            printf("'%u' ", node.child_id[j]);
        }
        printf("\n");

        nodes->nodes[node_idx++] = node;
    }

    printf("==============================\n\n");
    return node_idx;
}

void d2_print_tree( LocalTreeStore* nodes_out )
{
    for (int i = 0; i < nodes_out->number_of_nodes; ++i) {
        NetNode node = nodes_out->nodes[i];

        printf("\n==========================\n");
        printf("Id:\t(%d)\n", node.id);
        printf("Val:\t(%d)\n", node.value);
    }
    /* implement this */
}

