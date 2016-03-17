#include "../include/packet.h"
#include "../include/sr.h"
#include "../include/simulator.h"
#include <cstring>
#include <iostream>
#include <iterator>
#include <algorithm>

#define DEBUG_MODE 1 // Whether debugging mode is enabled or disabled
#define DEBUG(x)                                                               \
  do {                                                                         \
    if (DEBUG_MODE) {                                                          \
      std::cerr << x << " | time: " << get_sim_time() << std::endl;                                             \
    }                                                                          \
  } while (0) // http://stackoverflow.com/questions/14251038/debug-macros-in-c

/**
 * Create a new packet timer.
 * 
 * @param seq_num  The sequence number of the corresponding packet
 */
void new_pkt_timer(int seq_num) {
	if(pkt_timers.size() > MAX_NO_TIMERS) {
		// Too many active timers.
		DEBUG("packet timer: could not create a new timer for seq " << seq_num << " bececause there are too many timers currently running");
		return;
	}
	if(pkt_timer_exists(seq_num)) {
		DEBUG("packet timer: could not create new timer for seq " << seq_num << " because one already exists.");
		return;		
	}
	// Construct packet timer.
	struct pkt_timer timer;
	timer.seq_num = seq_num;
	timer.next_fire = get_sim_time() + PKT_TIMEOUT;
	timer.active = false;
	// Add packet timer to collection
	pkt_timers.push_back(timer);
}

/**
 * Start an existing packet timer.
 * 
 * @param seq_num  The sequence number of the corresponding packet
 */
void start_pkt_timer(int seq_num) {
	for(int i = 0; i < pkt_timers.size(); i++) {
		if(pkt_timers[i].seq_num == seq_num) {
			DEBUG("packet timer: starting timer for seq " << seq_num << " | next fire at " << get_sim_time() + PKT_TIMEOUT);
			pkt_timers[i].active = true;
			pkt_timers[i].next_fire = get_sim_time() + PKT_TIMEOUT;
			break;
		}
	}	
}

/**
 * Stop an existing packet timer.
 * 
 * @param seq_num  The sequence number of the corresponding packet
 */
void stop_pkt_timer(int seq_num) {
	for(int i = 0; i < pkt_timers.size(); i++) {
		if(pkt_timers[i].seq_num == seq_num) {
			DEBUG("packet timer: stopping timer for seq " << seq_num);
			pkt_timers[i].active = false;
			break;
		}
	}	
}

/**
 * Destroy a packet timer.
 * 
 * @param seq_num The sequence number of the corresponding packet
 */
void destroy_pkt_timer(int seq_num) {
	for(int i = 0; i < pkt_timers.size(); i++) {
		if(pkt_timers[i].seq_num == seq_num) {
			DEBUG("packet timer: destroyed timer for seq " << seq_num);
			pkt_timers.erase(pkt_timers.begin()+i);
			break;
		}
	}
}

/**
 * Fire a packet timer.
 *
 * @param seq_num The sequence number of the corresponding packet
 */
void fire_pkt_timer(int seq_num) {
	for(int i = 0; i < pkt_timers.size(); i++) {
		if(pkt_timers[i].seq_num == seq_num) {
			DEBUG("packet timer: timer for seq " << seq_num << " fired" << " because next_fire_time was " << pkt_timers[i].next_fire);
			pkt_timers[i].active = false;
			pkt_timer_interrupt_handler(seq_num);
		}
	}
}

/**
 * Fire all expired packet timers.
 */
void fire_expired_pkt_timers() {
	for(int i = 0; i < pkt_timers.size(); i++) {
		if(get_sim_time() >= pkt_timers[i].next_fire && pkt_timers[i].active) {
			fire_pkt_timer(pkt_timers[i].seq_num);
		}
	}
}

/**
 * Handle a packet timer fire event.
 * 
 * @param seq_num The sequence number of the corresponding packet
 */
void pkt_timer_interrupt_handler(int seq_num) {
	// In the case of the selective repeat (SR) protocol,
	// when a packet times out, it should be resent.
	resend_pkt(seq_num);
}

/**
 * Check whether a packet timer exists.
 * 
 * @param seq_num The sequence number of the corresponding packet
 * @return        true or false
 */
bool pkt_timer_exists(int seq_num) {
	for(int i = 0; i < pkt_timers.size(); i++) {
		if(pkt_timers[i].seq_num == seq_num) {
			return true;
		}
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
  new_pkt_timer(seqnum);	// Create packet timer
  return packet;
}

/**
 * Send a packet.
 * 
 * @param caller the sender, 0 for A, 1 for B
 * @param packet the packet to send
 */
void send_pkt(int caller, struct pkt packet) {
  // Send packet to receiver
  tolayer3(caller, packet);
  DEBUG("sender: packet sent | seq " << packet.seqnum);
  // Start packet timer
  start_pkt_timer(packet.seqnum);
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

/* called from layer 5, passed the data to be sent to other side */
void A_output(struct msg message)
{

}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(struct pkt packet)
{
	if(is_corrupt(packet)) {
		return;
	}
	DEBUG("sender: received ack " << packet.acknum);

}

/* called when A's timer goes off */
void A_timerinterrupt() {
	// Fire all expired packet timers
	fire_expired_pkt_timers();

  	// Restart hardware timer
  	starttimer(0, 1.0);
}  

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{
	base = 1;
	window_size = getwinsize();
  	// Start hardware timer
  	starttimer(0, 1.0);
}

/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(struct pkt packet)
{
	if(is_corrupt(packet)) {
		DEBUG("receiver: packet received but corrupted");
		return;
	}

}

/* the following rouytine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{

}
