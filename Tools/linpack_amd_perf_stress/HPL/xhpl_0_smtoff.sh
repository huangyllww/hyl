#!/bin/sh
export OMP_NUM_THREADS=8
export GOMP_CPU_AFFINITY="0 2 4 6 8 10 12 14"
export OMP_PROC_BIND=TRUE
export BLIS_IR_NT=1
export BLIS_JR_NT=1
export BLIS_IC_NT=2
export BLIS_JC_NT=4
numactl --cpunodebind=0 ./xhpl

