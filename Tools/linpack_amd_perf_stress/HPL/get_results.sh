#!/bin/bash

CP_ROOT_DIR=$(readlink -f $(dirname $0))
Tool=$CP_ROOT_DIR/tool
MachineCheck=$Tool/MachineCheck
FRE_LOG=$CP_ROOT_DIR/freLog
HPL_LOG=$CP_ROOT_DIR/log

logfile="$1"
curlooptime=$2

get_sysinfo()
{
   export SYS_ARCH=`cat /proc/cpuinfo | grep vendor_id | uniq | awk '{print $3}'`
   export SYS_NP=`cat /proc/cpuinfo | grep processor | wc -l`
   export SYS_NCPUS=`cat /proc/cpuinfo | grep 'physical id'  | sort | uniq | wc -l`
   export SYS_NCPUS_HT=`cat /proc/cpuinfo | grep 'physical id' | uniq | wc -l`
#   export SYS_FREQ=`cat /proc/cpuinfo | grep MHz | awk '{print $4}' | sort -n | tail -n 1`
   
   ave_freq=`cat $FRE_LOG/cpu_freq_monitor_"$curlooptime".log | sed -n '/AVE/p' | sort -n -k3`
   middle=`cat $FRE_LOG/cpu_freq_monitor_"$curlooptime".log | sed -n '/AVE/p' | wc -l`
   middle=`expr $middle / 2`
   middle_freq=`echo "$ave_freq" | sed -n "$middle"p | awk -F',' '{print $NF}'`
   export SYS_FREQ=`echo $middle_freq | sed 's/[[:space:]]//g'`
   if [[ "$SYS_ARCH" =~ AMD ]]; then
      export float_per_cycle=16
   elif [[ "$SYS_ARCH" =~ Intel ]]; then
      if cat /proc/cpuinfo | grep avx512 >& /dev/null; then
         export float_per_cycle=32
      elif cat /proc/cpuinfo | grep avx2 >& /dev/null; then
         export float_per_cycle=16
      elif cat /proc/cpuinfo | grep avx >& /dev/null; then
         export float_per_cycle=8
      else
         export float_per_cycle=4
      fi
   fi
   if [ "$SYS_NCPUS" -eq "$SYS_NCPUS_HT" ]; then
      # HyperThread is disabled
      export HPL_MAX_PERF=`echo "scale=2; $SYS_FREQ/1000*$SYS_NP*$float_per_cycle" | bc -l`
   else
      # HyperThread is enabled
      export HPL_MAX_PERF=`echo "scale=2; $SYS_FREQ/1000*$SYS_NP*$float_per_cycle/2" | bc -l`
   fi
}

get_sysinfo

cat <<EOF
==============================================================================================
T/V                N    NB     P     Q               Time                 Gflops    Efficiency
----------------------------------------------------------------------------------------------
EOF

grep -h WR $HPL_LOG/$logfile 2>/dev/null | sort -r -k 7 -n | \
awk -v perf=$HPL_MAX_PERF '{num+=1;total+=$7;printf("%s\t%.2f%s\n",$0,$7/perf*100,"%")}; \
   END{printf("%s%18.3e\t%.2f%s\n","----------------------------------------------------------------------------------------------\nAverage            -     -     -     -                  -     ",total/num,total/num/perf*100,"%")}' 

cat <<EOF
==============================================================================================
EOF

exit 0
