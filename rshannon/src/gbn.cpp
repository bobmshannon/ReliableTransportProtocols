#include "../include/packet.h"
#include "../include/gbn.h"
#include "../include/simulator.h"
#include <cstring>
#include <iostream>

#define DEBUG_MODE 1 // Whether debugging mode is enabled or disabled
#define DEBUG(x)                                                               \
  do {                                                                         \
    if (DEBUG_MODE) {                                                          \
      std::cerr << x << " | time: " << get_sim_time() << std::endl;                                             \
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
 * Send a packet.
 * @param caller the sender, 0 for A, 1 for B
 * @param packet the packet to send
 */
void send_pkt(int caller, struct pkt packet) {
  // Send packet to receiver
  tolayer3(caller, packet);
  DEBUG("sender: packet sent | seq " << packet.seqnum);
  if(base == next_seq_num) {
  	// Start timer
  	starttimer(caller, timer_interval);
  	DEBUG("sender: starting timer... | base " << base << " | next_seq_num " << next_seq_num);	
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

void add_to_unsent_buf(struct pkt packet) {
	if(unsent_buf.size() == MAX_BUF_SIZE) {
		// Enforce maximum buffer size of MAX_BUF_SIZE
		// If the buffer fills up, drop the oldest packet.
		unsent_buf.pop_front();
	}
	// Queue packet
	DEBUG("sender: adding packet " << packet.seqnum << " to unsent buffer");
	unsent_buf.push_back(packet);
	DEBUG("sender: unsent buffer has size " << unsent_buf.size());
}

void add_to_unacked_buf(struct pkt packet) {
	if(unacked_buf.size() == MAX_BUF_SIZE) {
		// Enforce maximum buffer size of MAX_BUF_SIZE
		// If the buffer fills up, drop the oldest packet.
		unacked_buf.pop_back();
	}
	// Queue packet
	DEBUG("sender: adding packet " << packet.seqnum << " to unacked buffer");
	unacked_buf.push_back(packet);
	DEBUG("sender: unacked buffer has size " << unacked_buf.size());
}

/* called from layer 5, passed the data to be sent to other side */
void A_output(struct msg message)
{
	struct pkt packet = make_pkt(next_seq_num, 0, message);
	if(next_seq_num < base + window_size) {
		send_pkt(0, packet);
		// Buffer unacknowledged packet
		add_to_unacked_buf(packet);
	} else {
		// Buffer unsent packet
		add_to_unsent_buf(packet);
	}
	next_seq_num++;
}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(struct pkt packet)
{
	if(is_corrupt(packet)) {
		return;
	}
	DEBUG("sender: received ack " << packet.acknum);
	base = packet.acknum + 1;
	if(unacked_buf.size() > 0) {
		unacked_buf.pop_front();
	} else if(unsent_buf.size() > 0) {
		struct pkt packet = unsent_buf.front();
		unsent_buf.pop_front();
		send_pkt(0, packet);
		// Buffer unacknowledged packet
		add_to_unacked_buf(packet);
	}
	if(base == next_seq_num) {
		stoptimer(0);
	}
}

/* called when A's timer goes off */
void A_timerinterrupt() {
	DEBUG("sender: timer interrupt fired");
  	// Resend all unacknowledged packets due
  	DEBUG("sender: resending all unacknowledged packets... | num_unacked " << unacked_buf.size() << " | base " << base << " | next_seq_num " << next_seq_num);
  	int i = 0;
  	while(i != unacked_buf.size()) {
  		// The oldest packet in the buffer
  		struct pkt packet = unacked_buf.front();
  		// Remove packet from buffer
  		unacked_buf.pop_front();
  		// Re-send packet
  		DEBUG("sender: resending unacked packet " << packet.seqnum);
  		tolayer3(0, packet);
  		// Re-add packet to buffer
  		unacked_buf.push_back(packet);
  		// Increment counter
  		i++;
  	}
  	// Start timer
  	starttimer(0, timer_interval);
  	DEBUG("sender: restarting timer... | base " << base << " | next_seq_num " << next_seq_num);
}  

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{
	base = 1;
	next_seq_num = 1;
	window_size = getwinsize();
	//timer_interval = window_size * 5.0;
	timer_interval = 15.0;
}

/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(struct pkt packet)
{
	if(is_corrupt(packet)) {
		DEBUG("receiver: packet received but corrupted");
		return;
	}
	if(packet.seqnum != expected_seq_num) {
		DEBUG("receiver: unexpected sequence number | expected " << expected_seq_num << " but got " << packet.seqnum);
		return;
	}
	// Deliver message to application
	tolayer5(1, packet.payload);
	// Send acknowledgement
	struct msg ack_data = {};
	struct pkt ack_pkt = make_pkt(expected_seq_num, expected_seq_num, ack_data);
	DEBUG("receiver: packet received, sending ack " << expected_seq_num);
	send_pkt(1, ack_pkt);
	expected_seq_num++;
}

//pkt make_pkt(int seqnum, int acknum, struct msg message) {

/* the following rouytine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{
	expected_seq_num = 1;
}
