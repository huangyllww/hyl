#!/bin/sh
export OMP_NUM_THREADS=16
export GOMP_CPU_AFFINITY="64 65 66 67 68 69 70 71 72 73 74 75 76 77 78 79"
export OMP_PROC_BIND=TRUE
export BLIS_IR_NT=1
export BLIS_JR_NT=1
export BLIS_IC_NT=2
export BLIS_JC_NT=8
numactl --cpunodebind=4 ./xhpl

