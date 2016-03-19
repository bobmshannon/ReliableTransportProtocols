#!/bin/bash

# In each of the following 2 experiments, run each of your protocols 
# with a total number of 1000 messages to be sent by entity A, 
# a mean time of 50 between message arrivals (from A’s layer5) 
# and a corruption probability of 0.2.

# Experiment 3
# With loss probability=0.0 and corruption=0.0, t=0.1, m=10000, compare the throughput
# of the 3 protocols with window sizes 10, 50, 100.

#################################################
# Alternating Bit Protocol (ABT)
#################################################
# Experiment 3, w=10
./run_experiments -o "Exp3-ABT-window-10.csv" -m 10000 -l 0.0 -c 0.0 -t 0.1 -w 10 -p ../rshannon/abt
# Experiment 3, w=50
./run_experiments -o "Exp3-ABT-window-50.csv" -m 10000 -l 0.0 -c 0.0 -t 0.1 -w 50 -p ../rshannon/abt
# Experiment 3, w=100
./run_experiments -o "Exp3-ABT-window-100.csv" -m 10000 -l 0.0 -c 0.0 -t 0.1 -w 100 -p ../rshannon/abt
# Experiment 3, w=200
./run_experiments -o "Exp3-ABT-window-200.csv" -m 10000 -l 0.0 -c 0.0 -t 0.1 -w 200 -p ../rshannon/abt
# Experiment 3, w=500
./run_experiments -o "Exp3-ABT-window-500.csv" -m 10000 -l 0.0 -c 0.0 -t 0.1 -w 500 -p ../rshannon/abt
#################################################
# Go-Back-N (GBN)
#################################################
# Experiment 2, w=10
./run_experiments -o "Exp3-GBN-window-10.csv" -m 10000 -l 0.0 -c 0.0 -t 0.1 -w 10 -p ../rshannon/gbn
# Experiment 2, w=50
./run_experiments -o "Exp3-GBN-window-50.csv" -m 10000 -l 0.0 -c 0.0 -t 0.1 -w 50 -p ../rshannon/gbn
# Experiment 2, w=100
./run_experiments -o "Exp3-GBN-window-100.csv" -m 10000 -l 0.0 -c 0.0 -t 0.1 -w 100 -p ../rshannon/gbn
# Experiment 2, w=200
./run_experiments -o "Exp3-GBN-window-200.csv" -m 10000 -l 0.0 -c 0.0 -t 0.1 -w 200 -p ../rshannon/gbn
# Experiment 2, w=500
./run_experiments -o "Exp3-GBN-window-500.csv" -m 10000 -l 0.0 -c 0.0 -t 0.1 -w 500 -p ../rshannon/gbn
#################################################
# Selective Repeat (SR)
#################################################
# Experiment 2, w=10
./run_experiments -o "Exp3-SR-window-10.csv" -m 10000 -l 0.0 -c 0.0 -t 0.1 -w 10 -p ../rshannon/sr
# Experiment 2, w=50
./run_experiments -o "Exp3-SR-window-50.csv" -m 10000 -l 0.0 -c 0.0 -t 0.1 -w 50 -p ../rshannon/sr
# Experiment 2, w=100
./run_experiments -o "Exp3-SR-window-100.csv" -m 10000 -l 0.0 -c 0.0 -t 0.1 -w 100 -p ../rshannon/sr
# Experiment 2, w=200
./run_experiments -o "Exp3-SR-window-200.csv" -m 10000 -l 0.0 -c 0.0 -t 0.1 -w 200 -p ../rshannon/sr
# Experiment 2, w=500
./run_experiments -o "Exp3-SR-window-500.csv" -m 10000 -l 0.0 -c 0.0 -t 0.1 -w 500 -p ../rshannon/sr


echo "Experiment script completed running" | dd of=status.txt