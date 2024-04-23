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
//void printbits(void *n, int size) {
//    char *num = (char *)n;
//
//    for (int i = size-1; i >= 0; i--) { // itererer gjennom bytes
//        for (int j = 7; j >= 0; j--) {
//            printf("%c", (num[i] & (1 << j)) ? '1' : '0');
//        }
//        printf(" ");
//    }
//    printf("\n");
//}


/**
 * Compute the checksum over the entire packet
 * @param header header-struct holding header-info
 * @param buffer payload
 * @param sz size of payload
 * @return
 */
uint16_t compute_checksum(D1Header header, const char* buffer, size_t sz) {

    uint16_t checksum = 0;

    // go through every 16 bit using pointer cast to 16-bit int
    const uint16_t *ptr = (uint16_t *)&header;
    for (size_t i = 0; i < (sizeof(D1Header) / sizeof(uint16_t)); ++i) {
        if (i == 1) continue; // skip checksum
        checksum ^= ptr[i];
    }

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

/**
 * Discover address info for server, store in D1Peer
 * @param peer to put info into
 * @param peername name to discover ip from
 * @param server_port port used
 * @return if success or not
 */
int d1_get_peer_info( struct D1Peer* peer, const char* peername, uint16_t server_port )
{
    struct sockaddr_in addr; // goes in peer
    struct in_addr ip_addr;  // goes in addr <- ip goes in here

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

int d1_recv_data(struct D1Peer* peer, char* buffer, size_t sz)
{
    if (sz < 8) return -1;

    // peer-info
    struct sockaddr_in src_addr = peer->addr;
    socklen_t addr_len = sizeof(struct sockaddr_in);

    // receives data in the buffer
    int bytes_received = recvfrom(peer->socket, buffer, sz, 0, (struct sockaddr*)&src_addr, &addr_len);
    printf("%d: Received %d bytes from %s\n", getpid(), bytes_received, inet_ntoa(peer->addr.sin_addr));
    if (bytes_received < 0) {
        perror("Error receiving data from socket");
        return -1;
    }

    // check if packet is right size
    if (bytes_received < (int)sizeof(D1Header)) {
        printf("Received data is smaller than the header size.\n");
        return -1;
    }

    // extract header info
    D1Header header;
    memcpy(&header.flags, buffer, sizeof(uint16_t));
    memcpy(&header.checksum, buffer + sizeof(uint16_t), sizeof(uint16_t));
    memcpy(&header.size, buffer + sizeof(uint32_t), sizeof(uint32_t));

    // extract payload: for calculating checksum
    size_t PAYLOAD_SIZE = (bytes_received - sizeof(D1Header));
    char *calc_payload = buffer + sizeof(D1Header);

    // extracted payload: to send
    char sendin_payload[PAYLOAD_SIZE];
    memcpy(&sendin_payload, buffer + sizeof(D1Header), PAYLOAD_SIZE);

    // calculate checksum using extracted payload
    uint16_t computed_checksum = compute_checksum(header, calc_payload, PAYLOAD_SIZE);

//    printf("\n\n=======[ Received info ]=======\n\n");
//    header.flags = ntohs(header.flags); header.size = ntohl(header.size);
//    printf("Flags:\t\t(%x)\t", header.flags); printbits(&header.flags, sizeof(uint16_t));
//    printf("Checksum:\t(%x)\t", header.checksum); printbits(&header.checksum, sizeof(uint16_t));
//    printf("Size:\t\t(%d)\t", header.size); printbits(&header.size, sizeof(uint32_t));
//    printf("\nCalc. checksum: (%x)\t", computed_checksum); printbits(&computed_checksum, sizeof(uint16_t));
//    header.flags = htons(header.flags); header.size = htonl(header.size);
//    printf("\n=======[ Received info ]=======\n\n");

    // convert to host byte order
    header.flags = htons(header.flags);
    header.size = htonl(header.size);

    // compare checksum and size
    int correct_checksum = (computed_checksum == header.checksum);
    int correct_size = (bytes_received == (int)header.size);

    // if data packet
    printf("%d: testing if data (%x) is set in flags (%x)\n", getpid(), FLAG_DATA, header.flags);
    if (header.flags & FLAG_DATA) {
        if (correct_checksum && correct_size) {
            printf("%d: Received a DATA packet with header %x %x %x, size correct, good checksum\n", getpid(), header.flags, header.size, header.checksum);

            // copy payload over to buffer
            memcpy(buffer, sendin_payload, PAYLOAD_SIZE);

            // gets seqno and sends an ack with same seqno
            int seqno = (header.flags & SEQNO) ? 1 : 0;
            d1_send_ack(peer, seqno);

            // returns size of payload
            return PAYLOAD_SIZE;
        }
        else {
            // checksum or size differs, send ack with opposite value
            printf("%d: Incorrect %s ...\n\n\n\n", getpid(), (correct_size ? "checksum" : "size"));
            int seqno = (header.flags & SEQNO) ? 0 : 1;
            d1_send_ack(peer, seqno);
            return -1;
            //TODO: double-check ?
        }
    }
    else {
        printf("%d: Received packet was not data packet ...\n", getpid());
        return -1;
    }
    return -1;
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

    // sends ACKNO with opposite sequence number back to server
    printf("%d: Sending an ACK packet, acking %d\n", getpid(), seqno);
    sendto(peer->socket, buffer, sz, 0, (struct sockaddr*)&(peer->addr), sizeof(peer->addr));
}

int  d1_wait_ack( D1Peer* peer, char* buffer, size_t sz ) {
    char ack_buff[8]; // ack packages are 8 bytes, only header
    socklen_t sender_addr_len = sizeof(peer->addr);

    int bytes_received = recvfrom(peer->socket, ack_buff, sizeof(ack_buff), 0, (struct sockaddr*)&(peer->addr), &sender_addr_len);
    if (bytes_received < 0) {
        perror("Error receiving acknowledgment");
        return -1;
    }

    printf("%d: Received %d bytes from %s\n", getpid(), bytes_received, inet_ntoa(peer->addr.sin_addr));

    D1Header *ack_header = (D1Header *)ack_buff;
    uint16_t ack_flags = ntohs(ack_header->flags);
    uint16_t ack_checksum = ack_header->checksum;
    uint32_t ack_size = ntohl(ack_header->size);

    // ackno and seqno comparison
    int ack_response = (ack_flags & ACKNO) ? 1 : 0;
    int expected_seqno = peer->next_seqno;
    int correct_acnko = (ack_response == expected_seqno);

//    printf("\n[ Sending - Ack received info ]\n");
//    printf("Flags\t\t(%x)\t", ack_flags); printbits(&ack_flags, sizeof(uint16_t));
//    printf("Checksum\t(%x)\t", ack_checksum); printbits(&ack_checksum, sizeof(uint16_t));
//    printf("Size\t\t(%d)\t", ack_size); printbits(&ack_size, sizeof(uint32_t));
//    printf("Received byted:\t%d\n", bytes_received);
//    printf("Ackno:\t\t%d\n", ack_response);
//    printf("Seqno:\t\t%d\n", expected_seqno);
//    printf("[ Sending - Ack received info ]\n\n");

    if (ack_flags & FLAG_ACK) {
        if (ack_size == bytes_received) { // size comparison
            if (correct_acnko) {          // ack comparison
                printf("%d: received frame with header %x %x %x, size correct - expected ack %d, advancing next seqno\n", getpid(), ack_flags, ack_size, ack_checksum, peer->next_seqno);
                peer->next_seqno = (peer->next_seqno ? 0 : 1);
                return 1;
            }
            else {
                printf("%d: received incorrect acknowledgement number ...\n", getpid());
                printf("%d: re-transmitting packet again\n", getpid());
                return d1_send_data(peer, buffer, sz);
            }
        }
        else {
            printf("%d: received incorrect size ...\n", getpid());
            return -1;
        }
    }
    else {
        printf("%d: received frame with header %x %x %x is not an ACK frame\n", getpid(), ack_flags, ack_size, ack_checksum);
        return -1;
    }
}

int d1_send_data( D1Peer* peer, char* buffer, size_t sz )
{
    // general error checks
    if (sz > 1016 || peer == NULL || buffer == NULL || sz == 0) return -1;

    printf("\n[>> Sending: '%.*s' <<]\n\n", (int)sz, buffer);

    // declares sizes
    const size_t PACKET_SIZE = (HEADER_SIZE + sz);

    // packet for holding header + data (buffer)
    // packet = [(header)(payload)]
    char packet[PACKET_SIZE];

    // creates the header and initializes it
    // data packet: DATA to 1 and ACK to 0
    D1Header header;
    header.flags = 0;
    header.checksum = 0;
    header.size = PACKET_SIZE;

    // marks flag-info and converts to network byte order, based on package and next seqno
    header.flags |= FLAG_DATA;
    if (peer->next_seqno == 1) header.flags |= SEQNO;

    // convert to network byte order before computing checksum
    header.flags = htons(header.flags);
    header.size = htonl(header.size);

    // computes the packets checksum
    header.checksum = compute_checksum(header, buffer, sz);

    // places header and data into the packet
    memcpy(packet, &header, HEADER_SIZE);
    memcpy(packet + HEADER_SIZE, buffer, sz);

    // sends the packet
    printf("%d: Sending a DATA packet with header %x %x %x, sending %d bytes\n", getpid(), ntohs(header.flags), ntohl(header.size), header.checksum, ntohl(header.size));
    printf("%d: Sending packet\n", getpid());
    int bytes_sent = sendto(peer->socket, packet, PACKET_SIZE, 0, (struct sockaddr*)&(peer->addr), sizeof(peer->addr));
    if (bytes_sent < 0) {
        perror("Error sending data");
        return -1;
    }

//    printf("\n[ Sending - packet info ]\n");
//    D1Header *test = (D1Header *)packet;
//    test->flags = ntohs(test->flags); test->size = ntohl(test->size);
//    printf("Flags\t\t(%x)\t", test->flags); printbits(&test->flags, sizeof(uint16_t));
//    printf("Checksum\t(%x)\t", test->checksum); printbits(&test->checksum, sizeof(uint16_t));
//    printf("Size\t\t(%d)\t", test->size); printbits(&test->size, sizeof(uint32_t));
//    printf("Bytes sent:\t%d\n", bytes_sent);
//    printf("Sending:\t'%.*s'\n", (int)sz, (char*)packet+sizeof(D1Header));
//    printf("[ Sending - packet info ]\n\n");

    // waits for an ACK
    int res = d1_wait_ack(peer, buffer, sz);

    if ( res ) return bytes_sent;
    else return -1;
}