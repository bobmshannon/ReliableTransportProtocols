#ifndef GBN_H_
#define GBN_H_

#include "../include/simulator.h"
#include <queue>

/**
 * The maximum amount of packets that the sender store in
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

/**
 * Go-Back-N (GBN) Sender (Computer Networking: A Top Down Approach, Kurose & Ross, pg 218)
 *
 * In Go-Back-N (GBN) protocol, the sender is allowed to transmit multiple
 * packets (when available) without waiting for an acknowledgement, but is
 * constrained to have no more than some maximum allowable number, N,
 * of unacknowledged packets in the pipeline. (Kurose & Ross 218).
 *
 * This implementation makes use of three book-keeping variables: base,
 * next_seq_num, and win_size, all of which are described in more detail
 * below.
 *
 * 1. base - The sequence number of the oldest unachnowledged packet.
 * 2. next_seq_num - The sequence number of the next packet to be sent.
 * 3. win_size - The maximum allowable number of unacknowledged packets in the
 *    pipeline.
 * 4. expected_seq_num - The next packet sequence number the sender expects to
 *    receive.
 *
 * Thus, the following intervals become significant:
 * 1. [0,base-1] - packets that have already been transmitted and acknowledged.
 * 2. [base, nextseqnum-1] - packets that have been sent but not yet
 *    acknowledged.
 * 3. [nextseqnum, base+win_size-1] - packets that can be sent immediately,
 *    should they arrive from the application layer.
 * 4. [base+win_size, upper_limit(seq_num_space_size)] - packets that cannot be sent
 *    because they are outside of the window size.
 */
int base;
int next_seq_num;
int window_size;
int expected_seq_num;

void send_pkt(int caller, struct pkt packet);

#endif