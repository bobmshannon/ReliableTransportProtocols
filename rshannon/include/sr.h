#ifndef SR_H_
#define SR_H_

#include "../include/simulator.h"
#include <queue>

/**
 * The maximum number of active packet timers.
 */
#define MAX_NO_TIMERS 1024

/**
 * Packet timeout every X time units.
 */
#define PKT_TIMEOUT 12.5

/**
 * A packet timer.
 */
struct pkt_timer {
  int seq_num; // The corresponding sequence number of the packet associated with
              // this timer
  float next_fire; // Next scheduled timer interrupt
  bool active;	// Whether this timer is active or not
};

/**
 * Container for all packet timers.
 */
std::vector<pkt_timer> pkt_timers;

/**
 * Helper methods to manage multiple packet timers
 * and their corresponding interrupt handlers.
 */
void new_pkt_timer(int seq_num);
void destroy_pkt_timer(int seq_num);
bool pkt_timer_exists(int seq_num);
void fire_pkt_timer(int seq_num);
void pkt_timer_interrupt_handler(int seq_num);
void fire_expired_pkt_timers();

/**
 * The maximum amount of packets that can be stored in
 * a single buffer.
 */
#define MAX_BUF_SIZE 1024

/**
 * Buffer containing all unacknowledged packets.
 */
std::deque<struct pkt> unacked_buf;

/**
 * Buffer containing all packets ready to be sent out
 * as soon as they are within the send window.
 */
std::deque<struct pkt> unsent_buf;

/**
 * Fire timer every X time units. This is a function
 * of the window size -- timer_interval = 5 * window_size
 */
float timer_interval;

/**
 * Helper methods to add a packet to a buffer.
 * Necessary in order to enforce a maximum queue size.
 */
void add_to_unsent_buf(struct pkt packet);
void add_to_unacked_buf(struct pkt packet);
bool sort_by_seq(const pkt& a, const pkt& b);

/**
 * Buffer which stores out of order packets on receiver side.
 */
std::vector<pkt> recv_buf;

/**
 * Selective-Repeat protocol book-keeping variables.
 */
int send_base;
int next_seq_num;
int window_size;
int recv_base;

/**
 * Send a packet through the network.
 * 
 * @param caller 0 for A, 1 for B
 * @param packet the packet to send
 */
void send_pkt(int caller, struct pkt packet);

/**
 * Re-send an unacknowledged packet.
 * 
 * @param seq_num the sequence number of the unacknowledged packet to re-send
 */
void resend_pkt(int seq_num);

#endif