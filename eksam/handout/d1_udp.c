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

    // adds padding if necessary
    char padded_buffer[sz+1];
    if (sz % 2 != 0) {
        memcpy(padded_buffer, buffer, sz);
        padded_buffer[sz] = 0;
        sz++;
    }

    // xor the payload-part
    const uint16_t *data_blocks = (const uint16_t *) padded_buffer;
    for (int i = 0; i < sz / sizeof(uint16_t); ++i) {
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
uint16_t flip_ackno(D1Header head) {
    if (head.flags & ACKNO)
        return head.flags &= ~ACKNO;
    else
        return head.flags |= ACKNO;
}

int d1_recv_data(struct D1Peer* peer, char* buffer, size_t sz)
{
//    printf("==== inside recv_data ====\n");
    if (sz < 8) return -1;
//    struct sockaddr_in src_addr = peer->addr;
//    socklen_t addr_len = sizeof(struct sockaddr_in);
//
//    // receives data in the buffer
//    int bytes_received = recvfrom(peer->socket, buffer, sz, 0, (struct sockaddr*)&src_addr, &addr_len);
//
//    // check for response
//    if (bytes_received < 0) {
//        perror("recvfrom in recv_data");
//        return -1;
//    }
//
//    // check if packet is right size
//    if (bytes_received < sizeof(D1Header)) {
//        printf("Received data is smaller than the header size.\n");
//        return -1;
//    }
//
//    // Extract the header and perform byte order conversion
//    // parse header and payload
//    D1Header header;
//    memcpy(&header, buffer, sizeof(D1Header));
//
//    // Extract the payload
//    size_t PAYLOAD_SIZE = bytes_received - sizeof(D1Header);
//    char payload[PAYLOAD_SIZE];
//    memcpy(&payload, buffer + sizeof(D1Header ), PAYLOAD_SIZE);
//
//    // extract and convert to host byte order
//    uint16_t flags = ntohs(header.flags);
//    uint16_t checksum = header.checksum;
//    uint32_t size = ntohl(header.size);
//
//    // calculates the checksum
//    uint16_t computed_checksum = compute_checksum(header, payload, PAYLOAD_SIZE);
//
//    // check if size or checksum is incorrect
//    if (size != bytes_received || computed_checksum != checksum) {
//        size_t PACKET_SIZE = (PAYLOAD_SIZE + sizeof(D1Header));
//        header.flags = flip_ackno(header);
//        char packet[PACKET_SIZE];
//        memcpy(packet, &header, HEADER_SIZE);
//        memcpy(packet + HEADER_SIZE, payload, PAYLOAD_SIZE);
//        int bytes_sent = sendto(peer->socket, packet, PACKET_SIZE, 0, (struct sockaddr*)&(peer->addr), sizeof(peer->addr));
//    }
//
//    return bytes_received;
    return 0;
}

int d1_wait_ack( D1Peer* peer, char* buffer, size_t sz )
{
    printf("\n========== wait ACK ==========\n");
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

    if (bytes_received > 0) {
        uint16_t flags;
        memcpy(&flags, ack_buffer, sizeof(uint16_t));
        flags = ntohs(flags);

        int received_ackno = (flags & ACKNO);
        int expected_seqno = peer->next_seqno;
        printf("Expected: %d\n", (peer->next_seqno));
        printf("Received: %d\n", (flags & ACKNO));
        printbits(&flags, sizeof(uint16_t));

        if (received_ackno == expected_seqno) {
            printf("\nChecksum matches\n");
            peer->next_seqno = (peer->next_seqno == 0) ? 1 : 0;
            return 1;
        } else {
            printf("\nChecksum differs\n");

            //TODO spør om dette her, uklar kommentar
            // + hva "alltid blokk etter sendt pakke til riktig ack", while-loop ?
            d1_send_data(peer, buffer, sz);
            d1_wait_ack(peer, buffer, sz);
        }
    }

//    // retrives flags from received packet
//    uint16_t flags = ntohs(*((uint16_t*)ack_buffer));
//    printf("Received flag\t"); printbits(&flags, sizeof(uint16_t)); printf("\n");
//
//    int received_seqno = (flags & ACKNO);
//    int expeceted_seqno = peer->next_seqno;
//
//    printf("Expected: %d\n", expeceted_seqno);
//    printf("Received: %d\n", received_seqno);
//
//    // if seqno matches ackno, flip next_seqno
//    if (received_seqno == expeceted_seqno) {
//        printf("\nAckno match, continuing ...\n");
//        peer->next_seqno = (peer->next_seqno == 1) ? 0 : 1;
//        return 1;
//    }
//    else {
//        printf("Re-sending packet ...\n");
//        int bytes_sent = d1_send_data(peer, buffer, sz);
//        if (bytes_sent < 0) {
//            perror("Error sending data (wait_ack)\n");
//            return -1;
//        }
//        printf("Re-waiting for ack ...\n");
//        int response = d1_wait_ack(peer, buffer, sz);
//        if (response < 0) {
//            perror("Error waiting for ACK (wait_ack)\n");
//            return -1;
//        }
//    }

    printf("\n====== end wait ack ========\n\n");
    return 1;
}


int d1_send_data( D1Peer* peer, char* buffer, size_t sz )
{
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

    //TODO: spør om dette er riktig, siden det står i UDP.h
    /* For connect, disconnect and ACK packets it is always 8. For data packets,
       it counts the bytes of the header and the data. */
    if (strcmp(buffer, "connect") == 0 || strcmp(buffer, "disconnect") == 0) {
        header.size = 8;
    }
    else header.size = PACKET_SIZE;

    // marks flag-info and converts to network byte order
    header.flags |= FLAG_DATA;

    //TODO: spør om SEQNO skal bli satt til 1 når man sender
    header.flags |= SEQNO;

    //TODO: konverteres byte order før checksum?
//    header.size = htonl(header.size);
    header.size = htonl(PACKET_SIZE);
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

    // Wait for ACK (optional?)
    int response = d1_wait_ack(peer, buffer, sz);
    if (response < 0) {
        perror("Error waiting for ACK");
        return -1;
    }

    return bytes_sent;
}

void d1_send_ack( struct D1Peer* peer, int seqno )
{
    /* implement this */
}
