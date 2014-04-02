Requirements:-

1. BLCR 0.8.2

2. OpenMPI > 1.3

Steps to be followed:-

1. Compile and execute JobScheduler.c on the scheduler node.

2. Compile and execute ResourceMonitor.c on the other nodes.

3. Compile start.c and give the process as a parameter to the resulting executable.

For running grid search....

1. mpicc ParallelizedgridSearch.c

2. mpirun -np <number of processes> --hostfile <filename.conf> <input_dataset> <output>


----> filename.conf should contain the node names in the following format...

node1
node2
.
.
.