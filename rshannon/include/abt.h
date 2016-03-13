#ifndef ABT_H_
#define ABT_H_

#include "../include/simulator.h"
#include <queue>

/**
 * Whether the most recently sent packet has been ACKed or not
 */
bool is_acked;

/**
 * The sequence number to use for outbound packets
 */
int current_seq_no;

/**
 * Queued outbound messages from application
 */
std::queue<struct msg> msg_queue;

/**
 * Buffer sent but unACKed packets
 */
struct pkt pkt_buf;

/**
 * Alternate between 0 and 1
 * 
 * @param  n the number to alternate, either 0 or 1
 * @return   0 or 1 depending on input
 */
int alternate_num(int n);

/**
 * Packetize and send all queued messages
 */
void clear_msg_queue();

void send_pkt(int caller, struct pkt packet);

#endif