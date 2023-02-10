#!/bin/sh
export OMP_NUM_THREADS=4
export GOMP_CPU_AFFINITY="16 18 20 22" 
export OMP_PROC_BIND=TRUE
export BLIS_IR_NT=1
export BLIS_JR_NT=1
export BLIS_IC_NT=1
export BLIS_JC_NT=4
numactl --physcpubind=16,18,20,22 --membind=1 ./xhpl

