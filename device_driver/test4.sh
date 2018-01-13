#!/bin/sh

echo -e "\n\n--------------------- Test Case 4 ------------------------\n"

echo -e "\nCreating two producer to produce 10 items and 5 items respectively followed by a consumer to consume 20 items.\n"

./producer.o 1 10 &
PID1=$!
./producer.o 2 5 &
PID2=$!
./consumer.o 1 20 &
PID3=$!

wait "$PID1" "$PID2" "$PID3"
