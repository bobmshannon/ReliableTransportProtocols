#include "../include/packet.h"
#include "../include/simulator.h"
#include <cstring>
#include <iostream>

/* ******************************************************************
 ALTERNATING BIT AND GO-BACK-N NETWORK EMULATOR: VERSION 1.1  J.F.Kurose

   This code should be used for PA2, unidirectional data transfer
   protocols (from A to B). Network properties:
   - one way network delay averages five time units (longer if there
     are other messages in the channel for GBN), but can be larger
   - packets can be corrupted (either the header or the data portion)
     or lost, according to user-defined probabilities
   - packets will be delivered in the order in which they were sent
     (although some can be lost).
**********************************************************************/

#define DEBUG_MODE 1 // Whether debugging mode is enabled or disabled
#define DEBUG(x)                                                               \
  do {                                                                         \
    if (DEBUG_MODE) {                                                          \
      std::cerr << x << std::endl;                                             \
    }                                                                          \
  } while (0) // http://stackoverflow.com/questions/14251038/debug-macros-in-c

/**
 * Construct a packet.
 *
 * @param  seqnum the sequence number of the packet
 * @param  acknum the ack number of the packet
 * @param  data   the packet payload
 * @return        a packet with calculated checksum
 */
pkt make_pkt(int seqnum, int acknum, struct msg message) {
  struct pkt packet = {}; // Initialize packet and zero fill members
  packet.seqnum = seqnum;
  packet.acknum = acknum;
  strncpy(packet.payload, message.data, MSG_LEN);
  packet.checksum = checksum(packet);
  return packet;
}

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
int checksum(struct pkt packet) {
  int checksum, i = 0;
  packet.checksum = 0;

  char *data = (char *)&packet;

  while (i < PACKET_LEN) {
  	checksum = (int)data[i];
  	i++;
  }

  return ~checksum;
}

/* called from layer 5, passed the data to be sent to other side */
void A_output(struct msg message) {
  // Construct packet
  struct pkt packet = make_pkt(0, 0, message);
  DEBUG("sender: packet constructed | " << "packet size: " << sizeof(packet) << " | checksum: " << packet.checksum);
}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(struct pkt packet) {}

/* called when A's timer goes off */
void A_timerinterrupt() {}

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init() {}

/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(struct pkt packet) {}

/* the following rouytine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init() {}
