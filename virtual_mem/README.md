# demand paged virtual memory

To run a particular case use following:

make
./virtmem <pages> <frames> rand|fifo|custom scan|sort|focus

...............................................................................

To generate the output for all the combination with 100 pages and frames 
starting from 10 and increase till 100 (step_size = 10), run the following
script,

./run.sh

Make sure that the script is given 777 permissions.

Outputs are present in output folder.
