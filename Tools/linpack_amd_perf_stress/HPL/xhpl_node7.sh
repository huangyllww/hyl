#!/bin/sh
export OMP_NUM_THREADS=11116
export GOMP_CPU_AFFINITY="56 57 58 59 60 61 62 63 120 121 122 123 124 125 126 127"
export OMP_PROC_BIND=TRUE
export BLIS_IR_NT=1
export BLIS_JR_NT=1
export BLIS_IC_NT=2
export BLIS_JC_NT=8
numactl --physcpubind=56,57,58,59,60,61,62,63,120,121,122,123,124,125,126,127 --membind=7 ./xhpl

