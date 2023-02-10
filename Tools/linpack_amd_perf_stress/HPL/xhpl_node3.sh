#!/bin/sh
export OMP_NUM_THREADS=11116
export GOMP_CPU_AFFINITY="24 25 26 27 28 29 30 31 88 89 90 91 92 93 94 95"
export OMP_PROC_BIND=TRUE
export BLIS_IR_NT=1
export BLIS_JR_NT=1
export BLIS_IC_NT=2
export BLIS_JC_NT=8
numactl --physcpubind=24,25,26,27,28,29,30,31,88,89,90,91,92,93,94,95 --membind=3 ./xhpl
