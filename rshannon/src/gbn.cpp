#include "../include/packet.h"
#include "../include/gbn.h"
#include "../include/simulator.h"
#include <cstring>
#include <iostream>
#include <algorithm>

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

bool sort_by_seq(const pkt& a, const pkt& b) {
    return a.seqnum < b.seqnum;
}

void unacked(struct pkt packet) {
	unacked_buf.push_back(packet);
	std::sort(unacked_buf.begin(), unacked_buf.end(), sort_by_seq);
}

void unsent(struct pkt packet) {
	unsent_buf.push_back(packet);
	std::sort(unsent_buf.begin(), unsent_buf.end(), sort_by_seq);
}

/* called from layer 5, passed the data to be sent to other side */
void A_output(struct msg message)
{
	if(next_seq_num < base + window_size) {
		struct pkt packet = make_pkt(next_seq_num, 0, message);
		DEBUG("sender: sent pkt " << next_seq_num);
		tolayer3(0, packet);
		next_seq_num++;
		unacked(packet);
		if(base == next_seq_num) {
			stoptimer(0);
			starttimer(0, timer_interval);
		}
	} else {
		struct pkt packet = make_pkt(next_seq_num, 0, message);
		unsent(packet);
	}
}

void cumulative_ack(int seq_num) {
	std::sort(unacked_buf.begin(), unacked_buf.end(), sort_by_seq);
	int last = 0;
	for(last = 0; last < unacked_buf.size(); last++) {
		if(unacked_buf[last].seqnum == seq_num) {
			break;
		}
	}
	for(int i = 0; i < last; i++) {
		unacked_buf.erase(unacked_buf.begin());
	}
}

void fill_sender_window() {
	int num_to_send = window_size - unacked_buf.size();
	int num_unsent = unsent_buf.size();
	if(num_to_send == 0 || unsent_buf.size() == 0) { return; }
	if(num_to_send > num_unsent) { num_to_send = num_unsent; }
	for(int i = 0; i < num_to_send; i++) {
		tolayer3(0, unsent_buf[i]);
		unacked(unsent_buf[i]);
	}
	for(int i = 0; i < num_to_send; i++) {
		unsent_buf.erase(unsent_buf.begin());
	}
}

void A_input(struct pkt packet)
{
	if(is_corrupt(packet)) {
		return;
	}
	base = packet.acknum + 1;
	cumulative_ack(packet.acknum);
	fill_sender_window();
	stoptimer(0);
	starttimer(0, timer_interval);
	//if(base == next_seq_num) {
	//	stoptimer(0);
	//	starttimer(0, timer_interval);
	//} //else {
	  //	stoptimer(0);
	  //	starttimer(0, timer_interval);
	  // }
}

void A_timerinterrupt() {
	for(int i = 0; i < unacked_buf.size(); i++) {
		DEBUG("sender: re-sending packet " << unacked_buf[i].seqnum << " due to timeout");
		tolayer3(0, unacked_buf[i]);
	}
	timer_interval += .1;
	starttimer(0, timer_interval);
}  

void A_init()
{
	base = 1;
	next_seq_num = 1;
	window_size = getwinsize();
	timer_interval = 11.0;
  	// Start hardware timer
  	starttimer(0, timer_interval);
}

void ack(int seq_num) {
	struct msg ack_msg = {};
	struct pkt ack_pkt = make_pkt(0, seq_num, ack_msg);
	tolayer3(1, ack_pkt);
}

void B_input(struct pkt packet)
{
	if(is_corrupt(packet)) {
		ack(expected_seq_num);
		return;
	}

	if(packet.seqnum == expected_seq_num) {
		tolayer5(1, packet.payload);
		ack(packet.seqnum);
		expected_seq_num ++;
		return;
	}

	ack(expected_seq_num);
}

void B_init()
{
	expected_seq_num = 1;
}
