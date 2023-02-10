#!/bin/sh
export OMP_NUM_THREADS=16
export GOMP_CPU_AFFINITY="16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31"
export OMP_PROC_BIND=TRUE
export BLIS_IR_NT=1
export BLIS_JR_NT=1
export BLIS_IC_NT=2
export BLIS_JC_NT=8
numactl --cpunodebind=1 ./xhpl

