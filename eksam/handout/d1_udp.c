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

    // allocate space for padded_buffer if needed
    const char *padded_buffer = buffer;
    size_t padded_sz = sz;
    if (sz % 2 != 0) {
        padded_sz = sz + 1;
        char *temp_buffer = malloc(padded_sz);
        if (temp_buffer == NULL) {
            perror("No space for temp_buffer in checksum computation");
            return -1;
        }
        memcpy(temp_buffer, buffer, sz);
        temp_buffer[sz] = 0;
        padded_buffer = temp_buffer;
    }

    // computate the buffer/payload-part
    const uint16_t *data_blocks = (const uint16_t*)padded_buffer;
    for (size_t i= 0; i < padded_sz / sizeof(uint16_t); i++) {
        checksum ^= data_blocks[i];
    }

    // free if padding necessary
    if (padded_buffer != buffer) {
        free((char*)padded_buffer);
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

    printf("%d: We have resolved server name %s\n", getpid(), peername);

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

    printf("%d: Received %d bytes from %s\n", getpid(), bytes_received, inet_ntoa(peer->addr.sin_addr));

    // extract payload
    int PAYLOAD_SIZE = (bytes_received - sizeof(D1Header));
    char payload[PAYLOAD_SIZE];
    memcpy(&payload, buffer + sizeof(D1Header), PAYLOAD_SIZE);

    // extract header info
    D1Header header;
    memcpy(&header.flags, buffer, sizeof(uint16_t));
    memcpy(&header.checksum, buffer + sizeof(uint16_t), sizeof(uint16_t));
    memcpy(&header.size, buffer + sizeof(uint32_t), sizeof(uint32_t));

    // calcualte checksum from buffer
    uint16_t calculated_checksum = compute_checksum(header, payload, PAYLOAD_SIZE);

    // convert to host byte order
    header.flags = htons(header.flags);
    header.checksum = header.checksum;
    header.size = htonl(header.size);

//    printbits(&header.flags, sizeof(uint16_t));
//    printf("Size: %d\n", header.size);
//    printf("Payload size: %zu\n", PAYLOAD_SIZE);
//    printf("Payload: %s%c\n", payload, '\0');
//    printf("Received checksum: %d\n", header.checksum);
//    printf("Calcualted checksum: %d\n", calculated_checksum);

    // if data packet
    if (header.flags & FLAG_DATA) {
        printf("%d: Received packet was a data packet.\n", getpid());
        printf("%d: Received a DATA packet with header %x %x %x, size correct, good checksum\n", getpid(), header.flags, header.size, header.checksum);

        // copy payload over to buffer
        memcpy(buffer, payload, PAYLOAD_SIZE);

        // gets seqno and sends an ack with same seqno
        int seqno = (header.flags & SEQNO) ? 1 : 0;
        d1_send_ack(peer, seqno);

        // returns size of payload
        return PAYLOAD_SIZE;
    }
    else {
        printf("%d: Received packet was not data packet ...\n", getpid());

        int seqno = (header.flags & SEQNO) ? 0 : 1;
        d1_send_ack(peer, seqno);
        return -1;
    }
}

void d1_send_ack( struct D1Peer* peer, int seqno )
{
    // ACKNO packet holder
    int sz = sizeof(D1Header);
    char buffer[sz];

    D1Header header;
    uint16_t flags = 0;
    uint32_t size = 8; // ACKNO size is always 8

    flags |= FLAG_ACK;
    if (seqno == 1) flags |= ACKNO;

    header.flags = htons(flags);
    header.checksum = 0;
    header.size = htonl(size);
    header.checksum = compute_checksum(header, NULL, 0);

    // move ACKNO header to buffer
    memcpy(buffer, &header, sizeof(D1Header));

    printf("%d: Sending an ACK packet, acking %d\n", getpid(), seqno);

    // sends ACKNO with opposite sequence number back to server
    sendto(peer->socket, buffer, sz, 0, (struct sockaddr*)&(peer->addr), sizeof(peer->addr));
}

int d1_wait_ack( D1Peer* peer, char* buffer, size_t sz )
{
//    printf("\n========== wait ACK ==========\n");
//
//    // peer info
//    int sockfd = peer->socket;
//    struct sockaddr_in src_addr;
//    socklen_t addr_len = sizeof(src_addr);
//
//    // Receive the ACK packet
//    char ack_buffer[MAX_PACKETSIZE];
//    int bytes_received = recvfrom(sockfd, ack_buffer, MAX_PACKETSIZE, 0, (struct sockaddr*)&src_addr, &addr_len);
//    if (bytes_received < 0) {
//        perror("Error receiving ACK");
//        return -1;
//    }
//
//    // extract the flags from received ack
//    uint16_t flags;
//    memcpy(&flags, ack_buffer, sizeof(uint16_t));
//    flags = ntohs(flags);
//
//    int received_ackno = (flags & ACKNO);
//    int expected_seqno = peer->next_seqno;
//    int correct_ack = (received_ackno == expected_seqno);
//
//    if (correct_ack) {
//        peer->next_seqno = (peer->next_seqno == 0 ? 1 : 0);
//        return 1;
//    }
//    else {
//        // tre-transmit the packet
//    }
//
//
//    printf("====== end wait ack ========\n");
//    return 1;
}


int d1_send_data( D1Peer* peer, char* buffer, size_t sz )
{
    // general error checks
    if (sz > 1016 || peer == NULL || buffer == NULL || sz == 0) return -1;

    printf("\n[>> Sending: '%.*s' <<]\n\n", (int)sz, buffer);

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
    header.flags |= (peer->next_seqno << 7); // choose seqno based on peers next

    // convert byte order before computing checksum
    header.size = htonl(header.size);
    header.flags = htons(header.flags);

    // computes the packets checksum and converts to network byte order
    header.checksum = compute_checksum(header, buffer, sz);

    // places header and data into the packet
    memcpy(packet, &header, HEADER_SIZE);
    memcpy(packet + HEADER_SIZE, buffer, sz);

    printf("%d: Sending a DATA packet with header %x %x %x, sending bytes %d\n", getpid(), ntohs(header.flags), ntohl(header.size), header.checksum, ntohl(header.size));

    // sends the packet
    int bytes_sent = sendto(peer->socket, packet, PACKET_SIZE, 0, (struct sockaddr*)&(peer->addr), sizeof(peer->addr));
    if (bytes_sent < 0) {
        perror("Error sending data");
        return -1;
    }

    // waits for an ACK
    char ack_buff[8]; // ack packages are 8 bytes, only header
    socklen_t sender_addr_len = sizeof(peer->addr);
    int bytes_received = recvfrom(peer->socket, ack_buff, sizeof(ack_buff), 0, (struct sockaddr*)&(peer->addr), &sender_addr_len);
    if (bytes_received < 0) {
        perror("Error receiving acknowledgment");
        return -1;
    }

    // extract header-info
    uint16_t flags; memcpy(&flags, ack_buff, sizeof(uint16_t));
    uint32_t size; memcpy(&size, ack_buff + sizeof(uint32_t), sizeof(uint32_t));

    int ack_response = (htons(flags) & ACKNO) ? 1 : 0;
    int expected_seqno = peer->next_seqno;
    int correct_ackno = (ack_response == expected_seqno);

//    printf("Ack flags:\t"); printbits(&flags, sizeof(uint16_t));
//    printf("ACKno:\t\t%d\n", ack_response);
//    printf("Seqno:\t\t%d\n", expected_seqno);

    // if correct ackno: flip peers next sequence number
    if (correct_ackno) {
        peer->next_seqno = (peer->next_seqno ? 1 : 0);
        return bytes_sent;
    }
    else {
         printf("Incorrect ackno!\n");
         printf("re-send package\n");
         d1_send_data(peer, buffer, sz);
         return -1;
    }

    // Wait for ACK
//    int response = d1_wait_ack(peer, packet, PACKET_SIZE);
//    if (response < 0) {
//        perror("Error waiting for ACK");
//        return -1;
//    }


    printf("====== end send data ========\n");
    return bytes_sent;
}