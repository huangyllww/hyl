#!/bin/sh
export OMP_NUM_THREADS=8
export GOMP_CPU_AFFINITY="48 49 50 51 52 53 54 55"
export OMP_PROC_BIND=TRUE
export BLIS_IR_NT=1
export BLIS_JR_NT=1
export BLIS_IC_NT=2
export BLIS_JC_NT=4
numactl --physcpubind=48,49,50,51,52,53,54,55 --membind=3 ./xhpl

