<h2> IN2140 - Home Exam </h2>

<h3>How my implementation works</h3>

<h4>D1</h4>

* D1 works by first creating a client and providing it info through several network functions. 
* "gethostbyname" was the choice i used to get ip from a hostname due to it being slightly easier than the alternative "getaddrinfo"

* recv_data and send_data uses respectively "recvfrom" and "sendto" together with a given buffer and a peer containing address info to send and receive data.
* In most cases the header and payload are extracted separately. The header is converted to right byte order and checked, and the payload is put back into the buffer for test file to receive data.


Overall the D1 program works by:
1. Creating a peer with info about server to send to 
2. The client sends a "connect" message to the server using "sendto" in d1_send_data
- This is read by the server and no response is sent back
- The client receives and ACK header which is extracted and checked for correct size, checksum and flag
3. A ping is sent to the server, which it acknowledges back
4. Client receives "pong" from server and verifies correctness

- This goes back and forth twice, until "disconnect" is received by server, in which case both stop

<h4>Checksum calculation</h4>

- Checksum is calculated in the d1_udp.c 
- The checksum calculator takes in a header and a payload together with its size, and iterates through both by casting it to 16 bit, then XOR'ing each part.
- There is also a check if the payload is uneven. In cases where the payload is uneven, e.g. odd number of letters in a string, a null-byte padding is added as to not ruin the checksum computation but still maintain 16-bit xor parts.

<h4>D2</h4>

- D2 reuses many of the functions created in D1.
- When creating a D2Client, d1_create_client together with d1_get_peer_info is called to fill it with the given parameter info.
- PacketHeaders are sent by creating a PacketHeader with appropriate flags and the given ID. Also made sure to add an empty 16 bit in the middle for the compiler. This PacketHeader is sent via d1_send_data
- PacketResponses and last responses are handled with d1_recv_data, and checks if the given data contains the right type of response.
- In the end, a tree is created based on the nodes collected in the payload of d2_recv_response, where a pre-created LocalTreeStore that contains an array with the size of how many nodes in the tree.
- The tree is array-based where the index of a given node corresponds to the nodes ID. This way, lookup for a specific node is done easily and hastily.
- printing is done via DFS recursively, where each printing is done first, then iteration through the current nodes children and then it re-calls the recursion on these.

- 
<h3>Elements not included / not implemented</h3>

- Everything according to the header-files and comments are implemented, and the program works well.

- In the case where a packet is received and the checksum or size is validated to be wrong, we send the opposite ack forcing sender to retransmit. After this ive called receive data again to capture this re-sent package. This might cause unexpected results due to limitations of testing with a binary server, but code-wise it makes sense for it to work.
