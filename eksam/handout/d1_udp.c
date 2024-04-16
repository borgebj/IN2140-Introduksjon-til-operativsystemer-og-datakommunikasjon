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

#define HEADER_SIZE sizeof(D1Header)

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
 * Compute the checksum over the entire packet
 * @param header header-struct holding header-info
 * @param buffer payload
 * @param sz size of payload
 * @return
 */
uint16_t compute_checksum(D1Header header, const char* buffer, size_t sz) {

    uint16_t checksum = 0;

    // checksums the haeader-fields
    checksum ^= header.flags;
    checksum ^= (uint16_t)(header.size & 0xFFFF); // upper size-field
    checksum ^= (uint16_t)(header.size >> 16);    // lower size-field

//    // calculate checksum of header
//    const uint16_t *header_blocks = (const uint16_t*)&header;
//    for (size_t i = 0; i < sizeof(D1Header) / sizeof(uint16_t); ++i) {
//        if (i == 1) continue;
//        checksum ^= header_blocks[i];
//    }

    // pad with 0 if uneven
    if (sz % 2 != 0) {
        char padded_buffer[sz + 1];
        memcpy(padded_buffer, buffer, sz);
        padded_buffer[sz] = 0;
        buffer = padded_buffer;
        sz += 1;
    }

    // xor the payload-part
    const uint16_t *data_blocks = (const uint16_t*)buffer;
    for (size_t i = 0; i < sz / sizeof(uint16_t); ++i) {
        checksum ^= data_blocks[i];
    }

    return checksum;
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
        memset(&(client->addr), 0, sizeof(struct sockaddr_in)); // initialized to 0
        client->next_seqno = 0; // initialized to 0
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

/**
 * Used for recv_data when incorrect size or checksum
 * Flips ackno for sending
 * @param head header file used in recv
 * @return flag with flipped ackno
 */
uint16_t flip_ackno(D1Header header) {
    return header.flags ^= ACKNO;
}

int d1_recv_data(struct D1Peer* peer, char* buffer, size_t sz)
{
    printf("==== inside recv_data ====\n");
    if (sz < 8) return -1;
    struct sockaddr_in src_addr = peer->addr;
    socklen_t addr_len = sizeof(struct sockaddr_in);

    // receives data in the buffer
    int bytes_received = recvfrom(peer->socket, buffer, sz, 0, (struct sockaddr*)&src_addr, &addr_len);
    if (bytes_received < 0) {
        perror("recvfrom in recv_data");
        return -1;
    }

    // check if packet is right size
    if (bytes_received < sizeof(D1Header)) {
        printf("Received data is smaller than the header size.\n");
        return -1;
    }

    // extract header
    D1Header header;
    memcpy(&header, buffer, sizeof(D1Header));

    // extract payload
    int PAYLOAD_SIZE = (bytes_received - sizeof(D1Header));
    char payload[PAYLOAD_SIZE];
    memcpy(&payload, buffer + sizeof(D1Header), PAYLOAD_SIZE);

    // calculate checksum before byte order conversion
    uint16_t calculated_checksum = compute_checksum(header, payload, PAYLOAD_SIZE);

    // convert byte order and store
    uint16_t checksum = header.checksum;
    uint32_t size = ntohl(header.size);

    /// check checksum
    uint16_t expected_checksum = checksum;
    uint16_t computed_checksum = calculated_checksum;
    int correct_checksum = (expected_checksum == computed_checksum);

    /// check size
    uint32_t expected_bytes = size;
    uint32_t received_bytes = bytes_received;
    int correct_size = (expected_bytes == received_bytes);

    // if both checksum and size are correct, send payload
    if (correct_checksum && correct_size) {

        // copy payload over to buffer
        memcpy(buffer, payload, PAYLOAD_SIZE);
        return PAYLOAD_SIZE;
    }
    else {
        printf("Checksum or size os incorrect ...\n");
        printf("Send ack with opposite value ...\n");
        uint16_t *flags_ptr = (uint16_t *)buffer;
        printf("Received flags:\t"); printbits(buffer, sizeof(uint16_t));
        int opposite = 0;
        d1_send_ack(peer, opposite);
    }

    return -1;
}

void d1_send_ack( struct D1Peer* peer, int seqno )
{
    /* implement this */
}

int d1_wait_ack( D1Peer* peer, char* buffer, size_t sz )
{
    printf("\n========== wait ACK ==========\n");

    // peer info
    int sockfd = peer->socket;
    struct sockaddr_in src_addr;
    socklen_t addr_len = sizeof(src_addr);

    // Receive the ACK packet
    char ack_buffer[PACKETSIZE];
    int bytes_received = recvfrom(sockfd, ack_buffer, PACKETSIZE, 0, (struct sockaddr*)&src_addr, &addr_len);
    if (bytes_received < 0) {
        perror("Error receiving ACK");
        return -1;
    }

    //TODO remove
    uint16_t flags;
    memcpy(&flags, ack_buffer, sizeof(uint16_t));
    uint16_t checksum;
    memcpy(&checksum, ack_buffer + sizeof(uint16_t), sizeof(uint16_t));
    uint32_t size;
    memcpy(&size, ack_buffer + sizeof(uint32_t), sizeof(uint32_t));
    flags = ntohs(flags);
    checksum = ntohs(checksum);
    size = ntohl(size);
    printf("[ Received info ]\n");
    printf("Flags:\t\t"); printbits(&flags, sizeof(uint16_t));
    printf("checksum:\t"); printbits(&checksum, sizeof(uint16_t));
    printf("Size: (%d)\t", size); printbits(&size, sizeof(uint16_t));
    printf("ACk packet: %d\n", (flags & FLAG_ACK) ? 1 : 0);
    printf("ACk number: %d\n", (flags & ACKNO) ? 1 : 0);
    printf("Peers next SEQNO: %d\n", peer->next_seqno);

    // if gotten ACK, parse and check
    if (bytes_received > 0) {

        uint16_t flags;
        memcpy(&flags, ack_buffer, sizeof(uint16_t));
        flags = ntohs(flags);

        int received_ackno = (flags & ACKNO);
        int expected_seqno = peer->next_seqno;
        int correct_ack = (received_ackno == expected_seqno);

        // if correct ACKNO, change peers next expected SEQNO
        if (correct_ack) {
            printf("\nACKNO matches SEQNO\n");
            peer->next_seqno = (peer->next_seqno == 0) ? 1 : 0;
            return 1;
        } else {
            printf("\nACKNO and SEQNO differs\n");
            //
        }
    }
    printf("\n====== end wait ack ========\n\n");
    return 1;
}


int d1_send_data( D1Peer* peer, char* buffer, size_t sz )
{
    printf("\n====== start send data ========\n");
    // general error checks
    if (sz > 1016 || peer == NULL || buffer == NULL || sz == 0) return -1;

    printf("\n[[ Sending: '%.*s' ]]\n", (int)sz, buffer);

    // declares sizes
    const size_t PACKET_SIZE = (HEADER_SIZE + sz);

    // packet for holding header + data (buffer)
    char packet[PACKET_SIZE];

    // creates the header and initializes it
    // data packet: DATA to 1 and ACK to 0
    D1Header header;
    header.flags = 0;
    header.checksum = 0;
    header.size = PACKET_SIZE;

    // marks flag-info and converts to network byte order
    header.flags |= FLAG_DATA;

//    TODO: uklart hvordan SEQNO skal håndteres
//    header.flags |= SEQNO; // set SEQNO since next is 0

    // convert byte order before computing checksum
    header.size = htonl(header.size);
    header.flags = htons(header.flags);

    // computes the packets checksum and converts to network byte order
    header.checksum = compute_checksum(header, buffer, sz);

    // places header and data into the packet
    memcpy(packet, &header, HEADER_SIZE);
    memcpy(packet + HEADER_SIZE, buffer, sz);

    // sends the packet
    int bytes_sent = sendto(peer->socket, packet, PACKET_SIZE, 0, (struct sockaddr*)&(peer->addr), sizeof(peer->addr));
    if (bytes_sent < 0) {
        perror("Error sending data");
        return -1;
    }

    // Wait for ACK
    int response = d1_wait_ack(peer, packet, PACKET_SIZE);
    if (response < 0) {
        perror("Error waiting for ACK");
        return -1;
    }

    printf("\n====== end send data ========\n\n");
    return bytes_sent;
}