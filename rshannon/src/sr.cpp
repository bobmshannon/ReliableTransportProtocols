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
      std::cerr << x << " | time: " << get_sim_time() << std::endl;            \
    }                                                                          \
  } while (0) // http://stackoverflow.com/questions/14251038/debug-macros-in-c

/**
 * Create a new packet timer.
 *
 * @param seq_num  The sequence number of the corresponding packet
 */
void new_pkt_timer(int seq_num) {
  if (pkt_timers.size() > MAX_NO_TIMERS) {
    // Too many active timers.
    DEBUG("packet timer: could not create a new timer for seq "
          << seq_num
          << " bececause there are too many timers currently running");
    return;
  }
  if (pkt_timer_exists(seq_num)) {
    DEBUG("packet timer: could not create new timer for seq "
          << seq_num << " because one already exists.");
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
  for (int i = 0; i < pkt_timers.size(); i++) {
    if (pkt_timers[i].seq_num == seq_num) {
      DEBUG("packet timer: starting timer for seq "
            << seq_num << " | next fire at " << get_sim_time() + PKT_TIMEOUT);
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
  for (int i = 0; i < pkt_timers.size(); i++) {
    if (pkt_timers[i].seq_num == seq_num) {
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
  for (int i = 0; i < pkt_timers.size(); i++) {
    if (pkt_timers[i].seq_num == seq_num) {
      DEBUG("packet timer: destroyed timer for seq " << seq_num);
      pkt_timers.erase(pkt_timers.begin() + i);
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
  for (int i = 0; i < pkt_timers.size(); i++) {
    if (pkt_timers[i].seq_num == seq_num) {
      DEBUG("packet timer: timer for seq " << seq_num << " fired"
                                           << " because next_fire_time was "
                                           << pkt_timers[i].next_fire);
      pkt_timers[i].active = false;
      pkt_timer_interrupt_handler(seq_num);
    }
  }
}

/**
 * Fire all expired packet timers.
 */
void fire_expired_pkt_timers() {
  for (int i = 0; i < pkt_timers.size(); i++) {
    if (get_sim_time() >= pkt_timers[i].next_fire && pkt_timers[i].active) {
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
  for (std::deque<pkt>::iterator it = unacked_buf.begin();
       it != unacked_buf.end();) {
    if ((*it).seqnum == seq_num) {
      DEBUG("sender: re-sending packet due to timeout... | seq "
            << (*it).seqnum);
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
  for (int i = 0; i < pkt_timers.size(); i++) {
    if (pkt_timers[i].seq_num == seq_num) {
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
  new_pkt_timer(seqnum); // Create packet timer
  return packet;
}

/**
 * Construct an ACK. This differs from make_pkt
 * in that it does not create a new packet timer.
 *
 * @param  seqnum the sequence number of the packet
 * @param  acknum the ack number of the packet
 * @param  data   the packet payload
 * @return        an ACK packet with calculated checksum
 */
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
  if (checksum_recv != checksum_calc) {
    return true;
  }
  return false;
}

/**
 * Add unacknowledged packet to buffer.
 *
 * @param packet the unacknowledged packet
 */
void add_to_unacked_buf(struct pkt packet) {
  if (unacked_buf.size() == MAX_BUF_SIZE) {
    // Enforce maximum buffer size of MAX_BUF_SIZE
    // If the buffer fills up, drop the oldest packet.
    unacked_buf.pop_back();
  }
  // Queue packet
  DEBUG("sender: adding packet " << packet.seqnum << " to unacked buffer");
  unacked_buf.push_back(packet);
  DEBUG("sender: unacked buffer has size " << unacked_buf.size());
}

/**
 * Add unsent packet to buffer.
 *
 * @param packet the unsent packet
 */
void add_to_unsent_buf(struct pkt packet) {
  if (unsent_buf.size() == MAX_BUF_SIZE) {
    // Enforce maximum buffer size of MAX_BUF_SIZE
    // If the buffer fills up, drop the oldest packet.
    unsent_buf.pop_front();
  }
  // Queue packet
  DEBUG("sender: adding packet " << packet.seqnum << " to unsent buffer");
  unsent_buf.push_back(packet);
  DEBUG("sender: unsent buffer has size " << unsent_buf.size());
}

/**
 * Called when an application has a message ready to sent
 * out through the network.
 *
 * @param message the message to send
 */
void A_output(struct msg message) {
  struct pkt packet = make_pkt(next_seq_num, 0, message);
  if (next_seq_num < send_base + window_size) {
    send_pkt(0, packet);
    // Buffer unacknowledged packet
    add_to_unacked_buf(packet);
  } else {
    // Buffer unsent packet
    add_to_unsent_buf(packet);
  }
  next_seq_num++;
}

/**
 * Called when host A received a packet from the network.
 *
 * @param packet the packet received from the network
 */
void A_input(struct pkt packet) {
  if (is_corrupt(packet)) {
    DEBUG("sender: received corrupted ack packet, ignoring...");
    return;
  }
  DEBUG("sender: received ack " << packet.acknum);

  // Update send_base
  std::sort(unacked_buf.begin(), unacked_buf.end(), sort_by_seq);
  if (unacked_buf[0].seqnum == packet.acknum) {
    if (unacked_buf.size() > 1) {
      send_base = unacked_buf[1].seqnum;
    } else {
      send_base++;
    }
  }
  DEBUG("BASE updated to " << send_base);

  // Mark packet as received
  int i;
  bool pkt_already_received = true;
  for (i = 0; i < unacked_buf.size(); i++) {
    if (unacked_buf[i].seqnum == packet.acknum) {
      pkt_already_received = false;
      break;
    }
  }
  if (!pkt_already_received) {
    destroy_pkt_timer(packet.acknum);
    unacked_buf.erase(unacked_buf.begin() + i);
  }

  // Send queued packets if there is space available in the window
  int free_to_send =
      window_size -
      unacked_buf.size(); // The number of new packets that can be sent
  int avail_to_send =
      unsent_buf.size(); // The number of queued packet available to be sent
  if (free_to_send > avail_to_send) {
    free_to_send = avail_to_send;
  }
  for (int i = 0; i < free_to_send; i++) {
    if (unsent_buf.size() == 0) {
      break;
    }
    struct pkt packet = unsent_buf.front();
    unsent_buf.pop_front();
    send_pkt(0, packet);
    // Buffer unacknowledged packet
    add_to_unacked_buf(packet);
  }
}

/**
 * Custom comparator. Sorts packets by sequence number,
 * from smallest to highest. Used with std::sort.
 */
bool sort_by_seq(const pkt &a, const pkt &b) { return a.seqnum < b.seqnum; }

/**
 * Called whenever a "hardware" timer interrupt occurs.
 * (Note that within a simulated environment like this
 * there is no true hardware timer present.)
 */
void A_timerinterrupt() {
  // Fire all expired packet timers
  fire_expired_pkt_timers();

  // Restart hardware timer
  starttimer(0, 1.0);
}

/**
 * Initialization for sender once simulation begins.
 */
void A_init() {
  send_base = 1;
  next_seq_num = 1;
  window_size = getwinsize();
  // Start hardware timer
  starttimer(0, 1.0);
}

/**
 * Called when a packet arrives at host B from the network.
 *
 * @param packet the packet from the network
 */
void B_input(struct pkt packet) {
  // Check if packet is corrupt
  if (is_corrupt(packet)) {
    DEBUG("receiver: packet received but corrupted");
    return;
  }

  // Check if packet already received
  for (int i = 0; i < recv_buf.size(); i++) {
    if (recv_buf[i].seqnum == packet.seqnum) {
      // Send acknowledgement
      struct pkt ack_pkt = make_ack_pkt(packet.seqnum, packet.seqnum);
      DEBUG("receiver: packet received, sending ack " << packet.seqnum);
      tolayer3(1, ack_pkt);
      return;
    }
  }

  // Packet is within receiver window
  if (packet.seqnum >= recv_base && packet.seqnum < recv_base + window_size) {
    // Send acknowledgement
    struct pkt ack_pkt = make_ack_pkt(packet.seqnum, packet.seqnum);
    DEBUG("receiver: packet received, sending ack " << packet.seqnum);
    tolayer3(1, ack_pkt);

    if (packet.seqnum == recv_base) {
      // Buffer received packet
      recv_buf.push_back(packet);
      // Deliver in order packets starting from recv_base
      int num_delivered = 0;
      std::sort(recv_buf.begin(), recv_buf.end(), sort_by_seq);
      for (int i = 0; i < recv_buf.size(); i++) {
        if (i == 0) {
          DEBUG("receiver: delivering packet " << recv_buf[i].seqnum);
          // Deliver packet
          tolayer5(1, recv_buf[i].payload);
          // Advance recv_base by number of packets delivered
          recv_base++;
          // Advance num_delivered
          num_delivered++;
          continue;
        }
        if (recv_buf[i - 1].seqnum + 1 == recv_buf[i].seqnum) {
          DEBUG("receiver: delivering packet " << recv_buf[i].seqnum);
          // Deliver packet
          tolayer5(1, recv_buf[i].payload);
          // Advance recv_base by number of packets delivered
          recv_base++;
          // Advance num_delivered
          num_delivered++;
          continue;
        } else {
          break;
        }
      }

      // Remove delivered packets from receiver buffer
      recv_buf.erase(recv_buf.begin(), recv_buf.begin() + num_delivered);
    } else {
      // Buffer out of order packet
      DEBUG("receiver: buffering out of order packet " << packet.seqnum);
      recv_buf.push_back(packet);
    }
  }
  // Send acknowledgement
  struct pkt ack_pkt = make_ack_pkt(packet.seqnum, packet.seqnum);
  DEBUG("receiver: packet received, sending ack " << packet.seqnum);
  send_pkt(1, ack_pkt);
}

/**
 * Initialization for receiver once simulation begins.
 */
void B_init() { recv_base = 1; }
