#!/bin/sh
export OMP_NUM_THREADS=11116
export GOMP_CPU_AFFINITY="32 33 34 35 36 37 38 39 96 97 98 99 100 101 102 103"
export OMP_PROC_BIND=TRUE
export BLIS_IR_NT=1
export BLIS_JR_NT=1
export BLIS_IC_NT=2
export BLIS_JC_NT=8
numactl --physcpubind=32,33,34,35,36,37,38,39,96,97,98,99,100,101,102,103 --membind=4 ./xhpl

