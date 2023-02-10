#!/bin/sh
export OMP_NUM_THREADS=8
export GOMP_CPU_AFFINITY="104 105 106 107 108 109 110 111"
export OMP_PROC_BIND=TRUE
export BLIS_IR_NT=1
export BLIS_JR_NT=1
export BLIS_IC_NT=2
export BLIS_JC_NT=4
numactl --physcpubind=104,105,106,107,108,109,110,111 --membind=6 ./xhpl

