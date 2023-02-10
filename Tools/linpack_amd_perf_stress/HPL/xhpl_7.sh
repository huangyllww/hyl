#!/bin/sh
export OMP_NUM_THREADS=16
export GOMP_CPU_AFFINITY="112 113 114 115 116 117 118 119 120 121 122 123 124 125 126 127"
export OMP_PROC_BIND=TRUE
export BLIS_IR_NT=1
export BLIS_JR_NT=1
export BLIS_IC_NT=2
export BLIS_JC_NT=8
numactl --cpunodebind=7 ./xhpl
