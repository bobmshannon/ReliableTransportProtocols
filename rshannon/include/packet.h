#ifndef PACKET_H_
#define PACKET_H_

#define PACKET_LEN 32  // The number of bytes which make up a packet structure, assuming a 
					   // a 32 bit word size and 8 bit chars.
#define MSG_LEN 20     // The number of bytes in each packet payload or message

/**
 * Take a packet structure and calculate
 * its checksum by adding up each 16 bit word
 * and flipping the result.
 *
 * Note that overflow bits are wrapped around
 * and added again.
 *
 * @param  packet the packet structure
 * @return        an integer checksum
 */
int checksum(struct pkt packet);
bool is_corrupt(struct pkt packet);

/**
 * Construct a packet.
 *
 * @param  seqnum the sequence number of the packet
 * @param  acknum the ack number of the packet
 * @param  data   the packet payload
 * @return        a packet with calculated checksum
 */
pkt make_pkt(int seqnum, int acknum, char data[]);
void send_pkt(int caller, struct pkt packet);


#endif