IN2140 - Exam
How my implementation works
D1 works by first creating a client and providing it info through several network functions.

gethostbyname was the choice i used to get ip from a hostname due to it being slightly easier than the alternative getaddrinfo

recv_data and send_data uses respectively "recvfrom" and "sendto" together with a given buffer and a peer containing address info to send and receive data.

In most cases the header and payload are extracted seperately. The header is converted to right byte order and checked, and the payload is put back into the buffer for test file to receive data.

Overall the program works by:

Creating a peer with info about server to send to
The client sends a "connect" message to the server using "sendto" in d1_send_data
This is interpreted by the server and no respons is sent back
The client receives and ACK header which is extracted and checked for correct size, checksum and flag
A ping is sent to the server, which it acknowledges back
Client receives "pong" from server and verifies correctness
This goes back and forth twice, until "disconnect" is received by server, in which case both stop
Checksum calculation
Checksum is calculated in the d1_udp.c
The checksum calculator takes in a header and a payload together with its size, and iterates through both by casting it to 16 bit, then XOR'ing each part.
There is also a check if the payload is uneven. In cases where the payload is uneven, e.g. odd number of letters in a string, a null-byte padding is added as to not ruin the checksum computation but still maintain 16-bit xor parts.
Elements not included / not implemented
Everything according to the header-files and comments are implemented, and the program works when run.
In the case where a packet is received and the checksum or size is validated to be wrong, we send the opposite ack forcing sender to retransmit. After this ive called receive data again to capture this re-sent package. This might caucse unexpected results due to limitations of testing with a binary server, but code-wise it makes sense for it to work.
