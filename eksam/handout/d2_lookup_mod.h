/* ======================================================================
 * YOU CAN MODIFY THIS FILE.
 * ====================================================================== */

#ifndef D2_LOOKUP_MOD_H
#define D2_LOOKUP_MOD_H

#include "d1_udp.h"

#define MAX_PACKETSIZE 1024
#define REQUEST_SIZE 64


struct D2Client
{
    D1Peer* peer;
};

typedef struct D2Client D2Client;

struct LocalTreeStore
{
    int number_of_nodes;
    struct NetNode *nodes; // array
};
typedef struct LocalTreeStore LocalTreeStore;

#endif /* D2_LOOKUP_MOD_H */

