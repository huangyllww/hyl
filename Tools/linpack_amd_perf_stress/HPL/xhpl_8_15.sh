#!/bin/sh
export OMP_NUM_THREADS=8
export GOMP_CPU_AFFINITY="8 9 10 11 12 13 14 15"
export OMP_PROC_BIND=TRUE
export BLIS_IR_NT=1
export BLIS_JR_NT=1
export BLIS_IC_NT=2
export BLIS_JC_NT=4
numactl --physcpubind=8,9,10,11,12,13,14,15 --membind=0 ./xhpl

