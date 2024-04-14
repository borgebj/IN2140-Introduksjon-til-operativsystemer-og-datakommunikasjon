/* ======================================================================
 * YOU ARE EXPECTED TO MODIFY THIS FILE.
 * ====================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "d1_udp.h"

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
 * Creates a UDP socket not bound to any port.
 * @return pointer to D1Peer-client on heap
 */
D1Peer* d1_create_client( )
{
    // creates the UDP socket
    int sockfd;
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) return NULL;

    // allocates space on heap for D1Peer, which is assigned socket and returned
    D1Peer* client = malloc(sizeof(D1Peer));
    if (client != NULL) {
        client->socket = sockfd;
        client->next_seqno = 1; // starts at 1
        return client;
    }
    return NULL;
}

/**
 * Deletes client from parameter, freeing its memory
 * @param peer node to delete
 * @return always returns NULL
 */
D1Peer* d1_delete( D1Peer* peer )
{
    // if non-NULL, close socket and free memory
    if (peer != NULL) {
        if (peer->socket != -1) { // shouldn't happen, but just in case
            close(peer->socket);
        }
        free(peer);
    }
    return NULL;
}

// Discover address info for server, store in D1Peer
int d1_get_peer_info( struct D1Peer* peer, const char* peername, uint16_t server_port )
{
    struct sockaddr_in addr;
    struct in_addr ip_addr;

    // convert to IP address if not in dotted
    int wc = inet_pton(AF_INET, peername, &ip_addr.s_addr);
    if (wc == 0) {
        // if dotted, get host by name through hostent struct
        struct hostent *host = gethostbyname(peername);
        if (host == NULL) {
            perror("gethostbyname");
            return 0;
        }
        // copies over ip address to the ip_addr
        memcpy(&ip_addr, host->h_addr, host->h_length);
    }
    else if (wc == -1) {
        perror("inet_pton");
        return 0;
    }

    // fills the sockaddr_in with found info
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(server_port);
    addr.sin_addr = ip_addr;

    // fills the peer with new structure containing IP
    memcpy(&(peer->addr), &addr, sizeof(addr));

    return 1;
}

int d1_recv_data( struct D1Peer* peer, char* buffer, size_t sz )
{
    /* implement this */
    return 0;
}

int d1_wait_ack( D1Peer* peer, char* buffer, size_t sz )
{
    printf("\n====== inside wait ack =====\n");
    /* This is meant as a helper function for d1_send_data.
     * When D1 data has send a packet, this one should wait for the suitable ACK.
     * If the arriving ACK is wrong, it resends the packet and waits again.
     *
     * Implementation is optional.
     */
    printf("\n====== end wait ack ========\n");
    return 0;
}

// Compute the checksum over the entire packet
uint16_t compute_checksum(D1Header header, const char* buffer, size_t sz) {

    // general error checks
    if (buffer == NULL || sz == 0) return 0;

    uint16_t checksum = 0;

    // calculate checksum of header
    const uint16_t *header_blocks = (const uint16_t*)&header;
    for (size_t i = 0; i < sizeof(D1Header) / sizeof(uint16_t); ++i) {
        checksum ^= header_blocks[i];
    }

    // pad with 0 if uneven
    if (sz % 2 != 0) {
        char padded_buffer[sz + 1];
        memcpy(padded_buffer, buffer, sz);
        padded_buffer[sz] = 0;
        buffer = padded_buffer;
        sz += 1;
    }

    // xor the data-part
    const uint16_t *data_blocks = (const uint16_t*)buffer;
    for (size_t i = 0; i < sz / sizeof(uint16_t); ++i) {
        checksum ^= data_blocks[i];
    }

    return checksum;
}

int d1_send_data( D1Peer* peer, char* buffer, size_t sz )
{
    // general error checks
    if (sz > 1016 || peer == NULL || buffer == NULL || sz == 0) return -1;

    // declares sizes
    const size_t HEADER_SIZE = sizeof(D1Header);
    const size_t PACKET_SIZE = (HEADER_SIZE + sz);

    // packet for holding header + data (buffer)
    char packet[PACKET_SIZE];

    // creates the header and initializes it
    // data packet: DATA to 1 and ACK to 0
    D1Header header;
    header.flags = 0;
    header.checksum = 0;
    header.size = htonl(PACKET_SIZE);

    // marks flag-info and converts to network byte order
    header.flags |= FLAG_DATA;
    if (peer->next_seqno) header.flags |= SEQNO;
    peer->next_seqno = (peer->next_seqno == 1) ? 0 : 1;
    header.flags = htons(header.flags);


    // computes the packets checksum and converts to network byte order
    header.checksum = compute_checksum(header, buffer, sz);
    header.checksum = htons(header.checksum);

    // places header and data into the packet
    memcpy(packet, &header, HEADER_SIZE);
    memcpy(packet + HEADER_SIZE, buffer, sz);

    printf("\n\nmsg: %s\n", buffer);
    printf("seqno: %d\n", (peer->next_seqno == 0 ? 1 : 0));
    printf("size: %d\n", header.size);
    printf("checksum: %d\n", header.checksum);

    // sends the packet
    int bytes_sent = sendto(peer->socket, packet, PACKET_SIZE, 0, (struct sockaddr*)&(peer->addr), sizeof(peer->addr));
    printf("Sent bytes: %d\n\n\n", bytes_sent);
//    d1_wait_ack(peer, packet, PACKET_SIZE);

    return bytes_sent;
}

void d1_send_ack( struct D1Peer* peer, int seqno )
{
    /* implement this */
}
