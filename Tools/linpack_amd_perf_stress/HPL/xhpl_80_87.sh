#!/bin/sh
export OMP_NUM_THREADS=8
export GOMP_CPU_AFFINITY="80 81 82 83 84 85 86 87"
export OMP_PROC_BIND=TRUE
export BLIS_IR_NT=1
export BLIS_JR_NT=1
export BLIS_IC_NT=2
export BLIS_JC_NT=4
numactl --physcpubind=80,81,82,83,84,85,86,87 --membind=5 ./xhpl

