/* ======================================================================
 * YOU ARE EXPECTED TO MODIFY THIS FILE.
 * ====================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "d2_lookup.h"

#define REQUEST_SIZE 64


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
    int bytes_received = d1_recv_data(client->peer, buffer, 1024);
    if( bytes_received < 0 ) {
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

/**
 * Receives response from server and puts into buffer provided
 * The payload is read from the buffer and stored into the b
 * @param client
 * @param buffer
 * @param sz
 * @return
 */
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

            // in case of success: returns bytes received
            return bytes_received;
        }
    }

    // fail-case: returns 0
    return 0;
}

/**
 * Allocates space for both the tree itself and its children
 * @param num_nodes how many nodes in the tree
 * @return tree-structure
 */
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

/**
 * Deletes all allocated spaces for and within the tree
 * @param nodes tree
 */
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
    // Pointer to traverse the buffer
    uint32_t *ptr = (uint32_t *)buffer;

    // Iterate through the buffer until no more bytes are left
    while ((char*)ptr < buffer + buflen) {
        NetNode node;

        // Convert byte order using ntohl for each field
        node.id = ntohl(*ptr++);
        node.value = ntohl(*ptr++);
        node.num_children = ntohl(*ptr++);

        // Convert each child as well
        uint32_t num_children = node.num_children;
        for (size_t j = 0; j < num_children && j < 5; ++j) {
            node.child_id[j] = ntohl(*ptr++);
        }

        // node is added to array-based tree, where ID represents index in array
        printf("%d: As tree node %u add id %u val %u num_children %u ", getpid(), node_idx, node.id, node.value, node.num_children);
        for (size_t i = 0; i < node.num_children; ++i) printf("%d ", node.child_id[i]);
        printf("\n");
        nodes->nodes[node_idx++] = node;
    }
    return node_idx;
}

/**
 * Recursive method to print each given node and its appropriate depth
 * @param nodes array in the tree containing all nodes
 * @param node_id ID of current node
 * @param depth depth to base print on, more depth = more "--"
 */
void print_recursive(NetNode *nodes, uint32_t node_id, int depth)
{
    // depth is represented by several more "--" before node info
    if (depth > 0) {
        for (int i = 0; i < depth; ++i) {
            printf("--");
        }
    }
    printf("id %d value %u children %d\n", nodes[node_id].id, nodes[node_id].value, nodes[node_id].num_children);

    // recursively go through each child of the current node
    for (size_t i = 0; i < nodes[node_id].num_children; ++i) {
        print_recursive(nodes, nodes[node_id].child_id[i], depth + 1);
    }
}

/**
 * Prints the tree using recursion
 * @param nodes_out tree
 */
void d2_print_tree( LocalTreeStore* nodes_out )
{
    // Prints the root
    printf("id %d value %u children %d\n", nodes_out->nodes[0].id, nodes_out->nodes[0].value, nodes_out->nodes[0].num_children);

    // starts the recursive descent from the root node
    for (size_t i = 0; i < nodes_out->nodes[0].num_children; ++i) {
        print_recursive(nodes_out->nodes, nodes_out->nodes[0].child_id[i], 1);
    }
}

// DFS print
///**
// * Prints the tree using a DFS approach
// * @param nodes_out  tree
// */
//void d2_print_tree(LocalTreeStore* nodes_out)
//{
//    int max_children = nodes_out->number_of_nodes;
//
//    // stack for DFS
//    NetNode* stack[max_children];
//    int stack_depth[max_children];
//    int stack_top = 0;
//
//    // Push root onto the stack
//    stack[stack_top] = &nodes_out->nodes[0];
//    stack_depth[stack_top++] = 0;
//
//    while (stack_top > 0) {
//        // Pop the top node from the stack
//        NetNode* current_node = stack[--stack_top];
//        int depth = stack_depth[stack_top];
//
//        // Print the current node
//        for (int i = 0; i < depth; ++i) {
//            printf("--");
//        }
//        printf(" id %d value %u children %d\n", current_node->id, current_node->value, current_node->num_children);
//
//        // Push children onto the stack in reverse order
//        for (int i = current_node->num_children - 1; i >= 0; --i) {
//            stack[stack_top] = &nodes_out->nodes[current_node->child_id[i]];
//            stack_depth[stack_top++] = depth + 1;
//        }
//    }
//}
