#!/bin/sh
export OMP_NUM_THREADS=16
export GOMP_CPU_AFFINITY="32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47"
export OMP_PROC_BIND=TRUE
export BLIS_IR_NT=1
export BLIS_JR_NT=1
export BLIS_IC_NT=2
export BLIS_JC_NT=8
numactl --cpunodebind=2 ./xhpl

