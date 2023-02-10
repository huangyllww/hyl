#!/bin/sh
export OMP_NUM_THREADS=4
export GOMP_CPU_AFFINITY="8 10 12 14"
export OMP_PROC_BIND=TRUE
export BLIS_IR_NT=1
export BLIS_JR_NT=1
export BLIS_IC_NT=1
export BLIS_JC_NT=4
numactl --physcpubind=8,10,12,14 --membind=0 ./xhpl

