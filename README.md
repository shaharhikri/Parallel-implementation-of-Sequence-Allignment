# Parallel-implementation-of-Sequence-Allignment
<h4>Running Instructions:<h4/>
MPI,OpenMP,CUDA Sequence Allignment program.

Author: Shahar Hikri

**This program was developed in a Linux environment (Ubuntu 18+MPI+OMP+CUDA), so it should be run on the same environment!
* Sequences (seq1 and seq2 in 'input.txt') should contain only capital letters ('A','B','C',...,'Z').

for run on 2 computers ('make runOn2'):
1. run 'hostname -I' command on two pc's.
2. copy the ip's of these 2 pc's to 'mf'.
3. make sure that 'mf' file in 2 pc's is the same.
4. run 'make'.
5. run 'make runOn2'.

for changing number of processes:
* on 'make run' - change value of 'RUN_PROC_NUM' (default value 18).
* on 'make runOn2' - change value of 'RUNON2_PROC_NUM' (default value 2).

How the program works:
* The program divides the processes into groups (communicators) as evenly as
  possible according to the number of processes and offsets.
* Each group receives an offset and calculates for it the optimal mutation (by swapping as many chars as possible - there can be more than one char!)
  and the score for it, these values ​​return to the master from all the groups and it takes
  the optimal combination of the seq2 and offset mutation.
* If there are fewer groups of possible offsets (happens only if there are fewer processes 
  than possible offsets), then the distribution of the offsets is done in a dynamic task pool.
- If there is more than one offset (seq1 longer than seq2) then the calculation
  (of the optimal seq2 mutation and its score) for the last offset is calculated by Koda in
   parallel with the calculations for the first offsets.
- If there is only one process he calculates for himself the optimal mutations
  for all possible offsets and takes the combination that gives optimal score by MPI & OMP.

dicionaries.h:
* has created by the program 'dictionaries_h_creation.py' (using python 3.8).
global arrays (dictionaries) for device(cuda gpu) and for host(cpu).

<h4>Exercise Description:<h4/>
![alt text](/paralel_readme.png?raw=true)

