#include "../include/packet.h"
#include "../include/abt.h"
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

#define DEBUG_MODE 0 // Whether debugging mode is enabled or disabled
#define DEBUG(x)                                                               \
  do {                                                                         \
    if (DEBUG_MODE) {                                                          \
      std::cerr << x << " | time: " << get_sim_time() << std::endl;                                             \
    }                                                                          \
  } while (0) // http://stackoverflow.com/questions/14251038/debug-macros-in-c

#define TIMER_INTERVAL 20.0 // Trigger timer interrupt every X time units

/**
 * Alternate a single sequence or ack number between
 * 0 and 1.
 *
 * @param  n  the number to alternate
 * @return    the alternated number 
 */
int alternate_num(int n) {
	if(n == 0) {
		return 1;
	} else if (n == 1) {
		return 0;
	}
	DEBUG("FATAL: invalid sequence or ack number provided; unable to alternate");
	return -1;
}

/**
 * Check whether a packet is corrupt.
 * 
 * In bizarre edge cases this could return
 * false positives, but it is unlikely.
 * 
 * @param  packet the packet to check
 * @return        true if corrupt, false otherwise
 */
bool is_corrupt(struct pkt packet) {
	int checksum_recv = packet.checksum;
	int checksum_calc = checksum(packet);
	if(checksum_recv != checksum_calc) {
		return true;
	}
	return false;
}

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

void send_pkt(int caller, struct pkt packet) {
  // Send packet to B
  tolayer3(0, packet);
  DEBUG("sender: packet sent | seq " << packet.seqnum);
  is_acked = false;
  // Buffer packet so that it can be re-sent if not ACKed by receiver
  pkt_buf = packet;
  // Start timer
  starttimer(0, TIMER_INTERVAL);
}

/**
 * Add message to outbound queue
 */
void queue_msg(struct msg message) {
	msg_queue.push(message);
}

/**
 * Packetize and send all queued messages
 */
void clear_msg_queue() {
	if(!msg_queue.empty()) {
		// Construct packet
		struct pkt packet = make_pkt(current_seq_no, 0, msg_queue.front());
		msg_queue.pop();
		DEBUG("sender: packet constructed | " << "packet size: " << sizeof(packet) << " | checksum: " << packet.checksum);
		// Send packet to B
		tolayer3(0, packet);
		DEBUG("sender: packet sent | seq " << packet.seqnum);
		is_acked = false;
		// Buffer packet so that it can be re-sent if not ACKed by receiver
		pkt_buf = packet;
		// Start timer
		starttimer(0, TIMER_INTERVAL);
	}
}

/**
 * Take a packet structure and calculate
 * its checksum by adding up each 8 bit chunk
 * of data.
 *
 * @param  packet the packet structure
 * @param  negate whether to negate the checksum or not
 * @return        an integer checksum
 */
int checksum(struct pkt packet) {
  int i = 0, checksum = 0;
  packet.checksum = 0;

  char *data = (char *)&packet;

  while (i < PACKET_LEN) {
  	checksum += (int)data[i];
  	i++;
  }

  return checksum;
}

/**
 * Called by application (layer 5) to send a message to client B.
 * 
 * @param message the message to send
 */
void A_output(struct msg message) {
  if(!is_acked) {
  	queue_msg(message);
  	return;
  }
  // Construct packet
  struct pkt packet = make_pkt(current_seq_no, 0, message);
  DEBUG("sender: packet constructed | " << "packet size: " << sizeof(packet) << " | checksum: " << packet.checksum);
  // Send packet
  send_pkt(0, packet);
}

/**
 * Called when packet arrives at client A from the network.
 * 
 * @param packet the packet to deliver
 */
void A_input(struct pkt packet) {
	// Ignore packets with unexpected ACK number
	if(packet.acknum != current_seq_no) {
		DEBUG("sender: packet received but wrong ACK number | sent seq " << current_seq_no << " but received ack " << packet.acknum);
		return;
	}
	// Check if packet is corrupted
	if(is_corrupt(packet)) {
		DEBUG("sender: ACK packet received but corrupted");
		return;
	}
	stoptimer(0);
	current_seq_no = alternate_num(current_seq_no);
	is_acked = true;
	DEBUG("sender: received ack " << packet.acknum);
	// Clear outbound message queue that may have built up while waiting for ACK
	clear_msg_queue();
}

/**
 * Sender side timer interrupt handler
 */
void A_timerinterrupt() {
	if(is_acked) {
		return;
	}
	DEBUG("sender: resending packet due to timeout | seq " << pkt_buf.seqnum);
	// Resend packet to receiver
	tolayer3(0, pkt_buf);
	// Start timer
	starttimer(0, TIMER_INTERVAL);
}


/**
 * Sender side initialization
 */
void A_init() {
	current_seq_no = 0;
	is_acked = true;
}

/**
 * Called when packet arrives at client B from network
 * 
 * @param packet the packet to deliver
 */
void B_input(struct pkt packet) {
	if(is_corrupt(packet)) {
		DEBUG("receiver: packet received but corrupted");
		return;
	}
	// Deliver message to application
	tolayer5(1, packet.payload);
	// Construct ACK packet
	struct msg message = {};
	struct pkt ack = make_pkt(0, packet.seqnum, message);
	// Send ACK
	tolayer3(1, ack);
	DEBUG("receiver: packet received, sending ack " << ack.acknum);
}

/* the following rouytine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init() {}
