#!/bin/sh
export OMP_NUM_THREADS=16
export GOMP_CPU_AFFINITY="96 97 98 99 100 101 102 103 104 105 106 107 108 109 110 111"
export OMP_PROC_BIND=TRUE
export BLIS_IR_NT=1
export BLIS_JR_NT=1
export BLIS_IC_NT=2
export BLIS_JC_NT=8
numactl --cpunodebind=6 ./xhpl

