#!/bin/bash

# In each of the following 2 experiments, run each of your protocols 
# with a total number of 1000 messages to be sent by entity A, 
# a mean time of 50 between message arrivals (from A’s layer5) 
# and a corruption probability of 0.2.

# Experiment 1
# With loss probabilities: {0.1, 0.2, 0.4, 0.6, 0.8}, compare the 3 
# protocols’ throughputs at the application layer of receiver B. Use 2 
# window sizes: {10, 50} for the Go-Back-N version and the 
# Selective-Repeat Version.

# Experiment 2
# With window sizes: {10, 50, 100, 200, 500} for GBN and SR, compare 
# the 3 protocols’ throughputs at the application layer of receiver B. 
# Use 3 loss probabilities: {0.2, 0.5, 0.8} for all 3 protocols.

#################################################
# Alternating Bit Protocol (ABT)
#################################################
# Experiment 2, l=0.2
./run_experiments -o "Exp2-ABT-loss-0.2.csv" -m 1000 -l 0.2 -c 0.2 -t 50 -w 10 -p ../rshannon/abt
# Experiment 2, l=0.5
./run_experiments -o "Exp2-ABT-loss-0.5.csv" -m 1000 -l 0.5 -c 0.2 -t 50 -w 10 -p ../rshannon/abt
# Experiment 2, l=0.8
./run_experiments -o "Exp2-ABT-loss-0.8.csv" -m 1000 -l 0.8 -c 0.2 -t 50 -w 10 -p ../rshannon/abt
#################################################
# Go-Back-N (GBN)
#################################################
# Experiment 2, l=0.2, w=10
./run_experiments -o "Exp2-GBN-loss-0.2-window-10.csv" -m 1000 -l 0.2 -c 0.2 -t 50 -w 10 -p ../rshannon/gbn
# Experiment 2, l=0.5, w=10
./run_experiments -o "Exp2-GBN-loss-0.5-window-10.csv" -m 1000 -l 0.5 -c 0.2 -t 50 -w 10 -p ../rshannon/gbn
# Experiment 2, l=0.8, w=10
./run_experiments -o "Exp2-GBN-loss-0.8-window-10.csv" -m 1000 -l 0.8 -c 0.2 -t 50 -w 10 -p ../rshannon/gbn

# Experiment 2, l=0.2, w=50
./run_experiments -o "Exp2-GBN-loss-0.2-window-50.csv" -m 1000 -l 0.2 -c 0.2 -t 50 -w 50 -p ../rshannon/gbn
# Experiment 2, l=0.5, w=50
./run_experiments -o "Exp2-GBN-loss-0.5-window-50.csv" -m 1000 -l 0.5 -c 0.2 -t 50 -w 50 -p ../rshannon/gbn
# Experiment 2, l=0.8, w=50
./run_experiments -o "Exp2-GBN-loss-0.8-window-50.csv" -m 1000 -l 0.8 -c 0.2 -t 50 -w 50 -p ../rshannon/gbn

# Experiment 2, l=0.2, w=100
./run_experiments -o "Exp2-GBN-loss-0.2-window-100.csv" -m 1000 -l 0.2 -c 0.2 -t 50 -w 100 -p ../rshannon/gbn
# Experiment 2, l=0.5, w=100
./run_experiments -o "Exp2-GBN-loss-0.5-window-100.csv" -m 1000 -l 0.5 -c 0.2 -t 50 -w 100 -p ../rshannon/gbn
# Experiment 2, l=0.8, w=100
./run_experiments -o "Exp2-GBN-loss-0.8-window-100.csv" -m 1000 -l 0.8 -c 0.2 -t 50 -w 100 -p ../rshannon/gbn

# Experiment 2, l=0.2, w=200
./run_experiments -o "Exp2-GBN-loss-0.2-window-200.csv" -m 1000 -l 0.2 -c 0.2 -t 50 -w 200 -p ../rshannon/gbn
# Experiment 2, l=0.5, w=200
./run_experiments -o "Exp2-GBN-loss-0.5-window-200.csv" -m 1000 -l 0.5 -c 0.2 -t 50 -w 200 -p ../rshannon/gbn
# Experiment 2, l=0.8, w=200
./run_experiments -o "Exp2-GBN-loss-0.8-window-200.csv" -m 1000 -l 0.8 -c 0.2 -t 50 -w 200 -p ../rshannon/gbn

# Experiment 2, l=0.2, w=500
./run_experiments -o "Exp2-GBN-loss-0.2-window-500.csv" -m 1000 -l 0.2 -c 0.2 -t 50 -w 500 -p ../rshannon/gbn
# Experiment 2, l=0.5, w=500
./run_experiments -o "Exp2-GBN-loss-0.5-window-500.csv" -m 1000 -l 0.5 -c 0.2 -t 50 -w 500 -p ../rshannon/gbn
# Experiment 2, l=0.8, w=500
./run_experiments -o "Exp2-GBN-loss-0.8-window-500.csv" -m 1000 -l 0.8 -c 0.2 -t 50 -w 500 -p ../rshannon/gbn
#################################################
# Selective Repeat (SR)
#################################################
# Experiment 2, l=0.2, w=10
./run_experiments -o "Exp2-SR-loss-0.2-window-10.csv" -m 1000 -l 0.2 -c 0.2 -t 50 -w 10 -p ../rshannon/sr
# Experiment 2, l=0.5, w=10
./run_experiments -o "Exp2-SR-loss-0.5-window-10.csv" -m 1000 -l 0.5 -c 0.2 -t 50 -w 10 -p ../rshannon/sr
# Experiment 2, l=0.8, w=10
./run_experiments -o "Exp2-SR-loss-0.8-window-10.csv" -m 1000 -l 0.8 -c 0.2 -t 50 -w 10 -p ../rshannon/sr

# Experiment 2, l=0.2, w=50
./run_experiments -o "Exp2-SR-loss-0.2-window-50.csv" -m 1000 -l 0.2 -c 0.2 -t 50 -w 50 -p ../rshannon/sr
# Experiment 2, l=0.5, w=50
./run_experiments -o "Exp2-SR-loss-0.5-window-50.csv" -m 1000 -l 0.5 -c 0.2 -t 50 -w 50 -p ../rshannon/sr
# Experiment 2, l=0.8, w=50
./run_experiments -o "Exp2-SR-loss-0.8-window-50.csv" -m 1000 -l 0.8 -c 0.2 -t 50 -w 50 -p ../rshannon/sr

# Experiment 2, l=0.2, w=100
./run_experiments -o "Exp2-SR-loss-0.2-window-100.csv" -m 1000 -l 0.2 -c 0.2 -t 50 -w 100 -p ../rshannon/sr
# Experiment 2, l=0.5, w=100
./run_experiments -o "Exp2-SR-loss-0.5-window-100.csv" -m 1000 -l 0.5 -c 0.2 -t 50 -w 100 -p ../rshannon/sr
# Experiment 2, l=0.8, w=100
./run_experiments -o "Exp2-SR-loss-0.8-window-100.csv" -m 1000 -l 0.8 -c 0.2 -t 50 -w 100 -p ../rshannon/sr

# Experiment 2, l=0.2, w=200
./run_experiments -o "Exp2-SR-loss-0.2-window-200.csv" -m 1000 -l 0.2 -c 0.2 -t 50 -w 200 -p ../rshannon/sr
# Experiment 2, l=0.5, w=200
./run_experiments -o "Exp2-SR-loss-0.5-window-200.csv" -m 1000 -l 0.5 -c 0.2 -t 50 -w 200 -p ../rshannon/sr
# Experiment 2, l=0.8, w=200
./run_experiments -o "Exp2-SR-loss-0.8-window-200.csv" -m 1000 -l 0.8 -c 0.2 -t 50 -w 200 -p ../rshannon/sr

# Experiment 2, l=0.2, w=500
./run_experiments -o "Exp2-SR-loss-0.2-window-500.csv" -m 1000 -l 0.2 -c 0.2 -t 50 -w 500 -p ../rshannon/sr
# Experiment 2, l=0.5, w=500
./run_experiments -o "Exp2-SR-loss-0.5-window-500.csv" -m 1000 -l 0.5 -c 0.2 -t 50 -w 500 -p ../rshannon/sr
# Experiment 2, l=0.8, w=500
./run_experiments -o "Exp2-SR-loss-0.8-window-500.csv" -m 1000 -l 0.8 -c 0.2 -t 50 -w 500 -p ../rshannon/sr

echo "Experiment script completed running" | dd of=status.txt