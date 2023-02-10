#!/bin/sh
export OMP_NUM_THREADS=8
export GOMP_CPU_AFFINITY="16 17 18 19 20 21 22 23"
export OMP_PROC_BIND=TRUE
export BLIS_IR_NT=1
export BLIS_JR_NT=1
export BLIS_IC_NT=2
export BLIS_JC_NT=4
numactl --physcpubind=16,17,18,20,21,22,23 --membind=1 ./xhpl

