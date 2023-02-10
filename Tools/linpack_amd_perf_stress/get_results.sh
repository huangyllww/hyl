#!/bin/bash

float_per_cycle=8
SYS_ARCH=`cat /proc/cpuinfo | grep vendor_id | uniq | awk '{print $3}'`
SYS_FREQ=`cpupower monitor | sed -n '3,$p' | sort -t'|' -n -r -k6  | head -n1 | awk -F'|' '{print $6}' | sed 's/[[:space:]]//g'`
SYS_NP=`cat /proc/cpuinfo | grep processor | wc -l`
if [[ "$SYS_ARCH" =~ AMD ]]; then
    V3_CPU_NAME=`cat /proc/cpuinfo | grep -i 'model name' | uniq | awk -F':' '{print $2}' | sed 's/^.//'`
    V3_CPU_TYPE=`cat /proc/cpuinfo | grep -i 'model name' | uniq | awk '{print $6}'`
    if [[ $V3_CPU_TYPE == "7601" ]]; then
      V3_CPU_BASE=2200
    elif [[ $V3_CPU_TYPE == "7551" ]]; then
      V3_CPU_BASE=2000
    elif [[ $V3_CPU_TYPE == "7551P" ]]; then
      V3_CPU_BASE=2000
    elif [[ $V3_CPU_TYPE == "7501" ]]; then
      V3_CPU_BASE=2000
    elif [[ $V3_CPU_TYPE == "7451" ]]; then
      V3_CPU_BASE=2300
    elif [[ $V3_CPU_TYPE == "7401" ]]; then
      V3_CPU_BASE=2000
    elif [[ $V3_CPU_TYPE == "7401P" ]]; then
      V3_CPU_BASE=2000
    elif [[ $V3_CPU_TYPE == "7351" ]]; then
      V3_CPU_BASE=2400
    elif [[ $V3_CPU_TYPE == "7351P" ]]; then
      V3_CPU_BASE=2400
    elif [[ $V3_CPU_TYPE == "7301" ]]; then
      V3_CPU_BASE=2200
    elif [[ $V3_CPU_TYPE == "7281" ]]; then
      V3_CPU_BASE=2100
    elif [[ $V3_CPU_TYPE == "7251" ]]; then
      V3_CPU_BASE=2100
    else
      V3_CPU_BASE=`echo $SYS_FREQ - 300 | bc`
      echo "Note: $V3_CPU_NAME not in knownlist, AVX base frequency minus 300 Hz for an estimation !!! "
      sleep 1
    fi
    echo "Note: $V3_CPU_NAME detected, AVX base frequency $V3_CPU_BASE Hz used in HPL/VASP efficiency !!! "
    sleep 1
    HPL_MAX_PERF=`echo "scale=2; $V3_CPU_BASE/1000*$SYS_NP*$float_per_cycle" | bc -l`
fi

cat <<EOF
==============================================================================================
T/V                N    NB     P     Q               Time                 Gflops    Efficiency
----------------------------------------------------------------------------------------------
EOF

grep -h WR HPL/log/*.out 2>/dev/null | sort -r -k 7 -n | \
awk -v perf=$HPL_MAX_PERF '{num+=1;total+=$7;printf("%s\t%.2f%s\n",$0,$7/perf*100,"%")}; \
   END{printf("%s%18.3e\t%.2f%s\n","----------------------------------------------------------------------------------------------\nAverage            -     -     -     -                  -     ",total/num,total/num/perf*100,"%")}' 

cat <<EOF
==============================================================================================
EOF

exit 0
