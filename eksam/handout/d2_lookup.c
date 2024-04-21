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
    if (id <= 1000) {
        printf("ID must be >1000 ...\n");
        return -1;
    }
    // TODO: remove, debu
    printf("\n\n[ send_request ]\n");

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
//    uint16_t pack_type = ntohs(*((uint16_t*)buffer));
//    uint16_t pack_empty = *((uint16_t*)(buffer + sizeof(uint16_t)));
//    uint32_t pack_id = ntohl(*((uint32_t*)(buffer + 2 * sizeof(uint16_t))));
//    printf("\n[ Before sending request, packet: ]\n");
//    printf("pack type:\t"); printbits(&pack_type, sizeof(uint16_t));
//    printf("pack empty:\t"); printbits(&pack_empty, sizeof(uint16_t));
//    printf("pack id:\t"); printbits(&pack_id, sizeof(uint32_t));
//    printf("[ Before sending request, packet: ]\n\n");

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

    printf("Type:\t"); printbits(&packet_type, sizeof(uint16_t));
    if (packet_type & TYPE_RESPONSE_SIZE) {

        // cast to responseSize to retrieve the size
        PacketResponseSize *responseSize = (PacketResponseSize *)buffer;
        uint16_t packet_size = ntohs(responseSize->size);

        // packet_size = how many NetNode structures that follows
        // The following PacketResponses contains NetNode as payload
        // netnode = (32 id, 32 val, 32 num, 32 child list)

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
    //TODO: or TPYE_LAST_RESPONSE ?
    if (packet_type == TYPE_RESPONSE || packet_type == TYPE_LAST_RESPONSE) {

        // cast to Response to retrieve data
        PacketResponse *response = (PacketResponse *)buffer;
        uint16_t payload_size = ntohs(response->payload_size);

        // check size retrieved
        if (payload_size > 0) {

            // Parse payload buffer into NetNode structure using 32-bit pointer
            NetNode node;
            uint32_t *ptr = (uint32_t *)(buffer + sizeof(PacketResponse));

            // retrieve and convert byte order from 32-bit pointer
            node.id = *ptr++;
            node.value = *ptr++;
            node.num_children = *ptr++;
            uint32_t nums = ntohl(node.num_children); // representing how many children

            // read in children-ids
            for (int i = 0; i < nums; ++i) {
                node.child_id[i] = *ptr++;
            }

            // put PacketResponse and NetNode into buffer -> buffer = [(response)(netnode)]
            memcpy(buffer, response, sizeof(PacketResponse));
            memcpy(buffer + sizeof(PacketResponse), &node, payload_size);

            //TODO: remove, debug
            uint16_t *bruh16 = (uint16_t *)buffer;
            printf("\n========== [ Header ] ===================================++===\n");
            printf("Flag:\t\t\t");
            printbits(&(uint16_t){ntohs(*bruh16++)}, sizeof(uint16_t));
            printf("Size:\t\t(%d)\t", ntohs(*bruh16));
            printbits(&(uint16_t){ntohs(*bruh16++)}, sizeof(uint16_t));
            uint32_t *bruh32 = (uint32_t *)bruh16;
            printf("========== [ Node ] ========================================\n");
            printf("ID:\t\t(%d)\t", ntohl(*bruh32));
            printbits(&(uint32_t){ntohl(*bruh32++)}, sizeof(uint32_t));
            printf("Value:\t\t(%d)\t", ntohl(*bruh32));
            printbits(&(uint32_t){ntohl(*bruh32++)}, sizeof(uint32_t));
            printf("Num_children:\t(%d)\t", ntohl(*bruh32));
            printbits(&(uint32_t){ntohl(*bruh32)}, sizeof(uint32_t));
            uint32_t children = ntohl(*bruh32++);
            for (int i=0; i < children; i++) {
                printf("Child %d:\t(%d)\t", i, ntohl(*bruh32));
                printbits(&(uint32_t){ntohl(*bruh32++)}, sizeof(uint32_t));
            }
            printf("============================================================\n\n");
            //TODO: remove, debug

            // in case of success: returns bytes received
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

int d2_add_to_local_tree( LocalTreeStore* nodes_out, int node_idx, char* buffer, int buflen )
{
    /* implement this */
    return 0;
}

void d2_print_tree( LocalTreeStore* nodes_out )
{
    /* implement this */
}

