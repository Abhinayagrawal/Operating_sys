#!  /bin/bash
mkdir output
for iter in {10..100..10}
do
    eval "./virtmem 100 " $iter "rand sort" >> output/rand_sort.txt
    eval "./virtmem 100 " $iter "rand scan" >> output/rand_scan.txt
    eval "./virtmem 100 " $iter "rand focus" >> output/rand_focus.txt
    eval "./virtmem 100 " $iter "fifo sort" >> output/fifo_sort.txt
    eval "./virtmem 100 " $iter "fifo scan" >> output/fifo_scan.txt
    eval "./virtmem 100 " $iter "fifo focus" >> output/fifo_focus.txt
    eval "./virtmem 100 " $iter "custom sort" >> output/custom_sort.txt
    eval "./virtmem 100 " $iter "custom scan" >> output/custom_scan.txt
    eval "./virtmem 100 " $iter "custom focus" >> output/custom_focus.txt
    #echo frames=$iter done
done
