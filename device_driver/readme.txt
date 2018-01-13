Group Members:
--------------
Aarti Sundararajan (5324659)
Abhinay Agrawal (5307098)
Anushree Jagrawal (5304406)


Compiling and installing the scullBuffer device:
--------------------------------------------------
1. Execute Make: 
   make

Now to run all the test cases, Run test_cases.sh to run all 4 test cases back to back.

To run individual tests:

1. If scull devices already exist then unload them first.
   sudo ./unload.py
   This may ask you for your root password.
2. Load scull devices
   sudo ./load.py scull_size=x
   This will create one scull buffer device: /dev/scullBuffer0
	where x is the number of items.
3. Run the test<1,2,3,4>. sh script. 

----------------------------------------------------
To run a producer, ./producer.o <producer-id> <number-of-items>
To run a consumer, ./consumer.o <consumer-id> <number-of-items>

