#!/bin/sh

echo -e "\n\n--------------------- Test Case 2 ------------------------\n"

echo -e "\nCreating producer to produce 10 items followed by consumer to consume 15 items.\n"
./producer.o 1 10 &
PID1=$!
./consumer.o 1 15 &
PID2=$!
wait "$PID1" "$PID2"

