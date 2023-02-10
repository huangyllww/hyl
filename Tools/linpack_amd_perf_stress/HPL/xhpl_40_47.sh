#!/bin/sh
export OMP_NUM_THREADS=8
export GOMP_CPU_AFFINITY="40 41 42 43 44 45 46 47"
export OMP_PROC_BIND=TRUE
export BLIS_IR_NT=1
export BLIS_JR_NT=1
export BLIS_IC_NT=2
export BLIS_JC_NT=4
numactl --physcpubind=40,41,42,43,44,45,46,47 --membind=2 ./xhpl

