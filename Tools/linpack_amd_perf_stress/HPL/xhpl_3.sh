#!/bin/sh
export OMP_NUM_THREADS=16
export GOMP_CPU_AFFINITY="48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63"
export OMP_PROC_BIND=TRUE
export BLIS_IR_NT=1
export BLIS_JR_NT=1
export BLIS_IC_NT=2
export BLIS_JC_NT=8
numactl --cpunodebind=3 ./xhpl

