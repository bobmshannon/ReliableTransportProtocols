#include "../include/packet.h"
#include "../include/sr.h"
#include "../include/simulator.h"
#include <cstring>
#include <iostream>
#include <iterator>
#include <algorithm>

#define DEBUG_MODE 0 // Whether debugging mode is enabled or disabled
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
 * Resend a buffered packet.
 *
 * @param seq_num The sequence number of the packet to resend
 */
void resend_pkt(int seq_num) {
	DEBUG("sender: unacked buf size " << unacked_buf.size());
	for(std::deque<pkt>::iterator it = unacked_buf.begin(); it != unacked_buf.end(); ) {
		if((*it).seqnum == seq_num) {
			DEBUG("sender: re-sending packet due to timeout... | seq " << (*it).seqnum);
			send_pkt(0, (*it));
		}
		it++;
	}
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

pkt make_ack_pkt(int seqnum, int acknum) {
  struct pkt packet = {}; // Initialize packet and zero fill members
  struct msg ack_msg = {};
  packet.seqnum = seqnum;
  packet.acknum = acknum;
  strncpy(packet.payload, ack_msg.data, MSG_LEN);
  packet.checksum = checksum(packet);
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

/* called from layer 5, passed the data to be sent to other side */
void A_output(struct msg message)
{
	struct pkt packet = make_pkt(next_seq_num, 0, message);
	if(next_seq_num < send_base + window_size) {
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
		DEBUG("sender: received corrupted ack packet, ignoring...");
		return;
	}
	DEBUG("sender: received ack " << packet.acknum);

	// Mark packet as received
	int i;
	bool pkt_already_received = true;
	for(i = 0; i < unacked_buf.size(); i++) {
		if(unacked_buf[i].seqnum == packet.acknum) {
			pkt_already_received = false;
			break;
		}
	}
	if(!pkt_already_received) { unacked_buf.erase(unacked_buf.begin()+i); }

	// Update window base
	if(packet.acknum == send_base) {
		std::sort(unacked_buf.begin(), unacked_buf.end(), sort_by_seq);
		send_base = unacked_buf[0].seqnum;

		// Send a queued packet if there is space available in the window
		int free_to_send = window_size - unacked_buf.size();	// The number of new packets that can be sent
		int avail_to_send = unsent_buf.size();					// The number of queued packet available to be sent

		for(int i = 0; i < free_to_send; i++) {
			if(unsent_buf.size() == 0) { break; }
			struct pkt packet = unsent_buf.front();
			unsent_buf.pop_front();
			send_pkt(0, packet);
			// Buffer unacknowledged packet
			add_to_unacked_buf(packet);
		}
	}
	
}

bool sort_by_seq(const pkt& a, const pkt& b) {
    return a.seqnum < b.seqnum;
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
	send_base = 1;
	next_seq_num = 1;
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

	if(packet.seqnum >= recv_base && packet.seqnum < recv_base + window_size) {
		// Send acknowledgement
		struct pkt ack_pkt = make_ack_pkt(packet.seqnum, packet.seqnum);
		DEBUG("receiver: packet received, sending ack " << packet.seqnum);
		send_pkt(1, ack_pkt);

		// packet is within receiver window
		if(packet.seqnum == recv_base) {
			// deliver in order packets starting from recv_base
			int last = 0;
			std::sort(recv_buf.begin(), recv_buf.end(), sort_by_seq);
			for(int i = 0; i < recv_buf.size(); i++) {
				if(i == 0) { continue; }
				if(recv_buf[i-1].seqnum+1 != recv_buf[i].seqnum) {
					break;
				} else {
					last = i-1;
				}
			}
			for(int i = 0; i <= last; i++) {
				// deliver packet
				tolayer5(1, recv_buf[i].payload);
				// advance recv_base by number of packets delivered
				recv_base++;
			}
			// clear recv buf
			if(recv_buf.size() > 0) {
				if(last == 0) { recv_buf.erase(recv_buf.begin()); }
				else { recv_buf.erase(recv_buf.begin(), recv_buf.begin()+last+1); }
			}

		} else {
			// buffer out of order packet
			recv_buf.push_back(packet);
		}
	} else if(packet.seqnum >= recv_base - window_size && packet.seqnum < recv_base) {
		// Send acknowledgement
		struct pkt ack_pkt = make_ack_pkt(packet.seqnum, packet.seqnum);
		DEBUG("receiver: packet received, sending ack " << packet.seqnum);
		send_pkt(1, ack_pkt);
	}

}

/* the following rouytine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{
	recv_base = 1;
}
