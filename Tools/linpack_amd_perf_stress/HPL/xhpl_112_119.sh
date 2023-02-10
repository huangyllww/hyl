#!/bin/sh
export OMP_NUM_THREADS=8
export GOMP_CPU_AFFINITY="112 113 114 115 116 117 118 119"
export OMP_PROC_BIND=TRUE
export BLIS_IR_NT=1
export BLIS_JR_NT=1
export BLIS_IC_NT=2
export BLIS_JC_NT=4
numactl --physcpubind=112,113,114,115,116,117,118,119 --membind=7 ./xhpl