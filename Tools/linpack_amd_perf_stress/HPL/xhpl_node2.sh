#!/bin/sh
export OMP_NUM_THREADS=11116
export GOMP_CPU_AFFINITY="16 17 18 19 20 21 22 23 80 81 82 83 84 85 86 87"
export OMP_PROC_BIND=TRUE
export BLIS_IR_NT=1
export BLIS_JR_NT=1
export BLIS_IC_NT=2
export BLIS_JC_NT=8
numactl --physcpubind=16,17,18,19,20,21,22,23,80,81,82,83,84,85,86,87 --membind=2 ./xhpl

