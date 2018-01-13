#!/bin/sh

echo -e "\n\n--------------------- Test Case 3 ------------------------\n"

echo -e "\nCreating producer to produce 10 items followed by two consumers to consume 6 and 8 items respectively.\n"

./producer.o 1 10 &
PID1=$!
./consumer.o 1 6 &
PID2=$!
./consumer.o 2 8 &
PID3=$!

wait "$PID1" "$PID2" "$PID3"

