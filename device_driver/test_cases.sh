#!/bin/sh

module="scullBuffer"

if [ "$(id -u)" != "0" ]; then
	echo "Sorry, it can be executed with root only! Please execute with sudo permissions."
	exit 1
fi

echo -e "\tDoing unload, load and setting permissions for scullbuffer device driver.\n"
chmod 755 ./*

if lsmod | grep "$module" &> /dev/null ; then
    python ./unload.py
fi

python ./load.py scull_size=10
chmod 777 /dev/scull*

echo -e "\n\nExecuting all the test cases for the sculldriver.\n\n"

sleep 1.5

( . ./test1.sh )
echo -e "\nTest Case 1 done.\n"
sleep 1

( . ./test2.sh )
echo -e "\nTest Case 2 done.\n"
sleep 1

( . ./test3.sh )
echo -e "\nTest Case 3 done.\n"
sleep 1

( . ./test4.sh )
echo -e "\nTest Case 4 done.\n"

echo -e "\nAll test cases are finished.\n\n"
echo -e "Now, unloading the scull module."
python ./unload.py

