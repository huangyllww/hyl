#!/bin/sh
export OMP_NUM_THREADS=16
export GOMP_CPU_AFFINITY="80 81 82 83 84 85 86 87 88 89 90 91 92 93 94 95"
export OMP_PROC_BIND=TRUE
export BLIS_IR_NT=1
export BLIS_JR_NT=1
export BLIS_IC_NT=2
export BLIS_JC_NT=8
numactl --cpunodebind=5 ./xhpl

