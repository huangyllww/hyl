#!/bin/sh
sleep 10
#CPUMAP="0,1,2,3"
#CPUMAP="0,1,2,3,4,5,6,7"
#CPUMAP="0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15"
cpubine=`numactl --show | awk -F':' '/physcpubind/ {print $2}' | sed 's/^.//' | sed 's/ /,/g'| sed 's/.$//'`
CPUMAP="$cpubine"
#CPUMAP="0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127"
#CPUMAP="0,2,4,6,8,10,12,14,16,18,20,22,24,26,28,30,32,34,36,38,40,42,44,46,48,50,52,54,56,58,60,62,64,66,68,70,72,74,76,78,80,82,84,86,88,90,92,94,96,98,100,102,104,106,108,110,112,114,116,118,120,122,124,126"

NUMCORE=`echo ${CPUMAP} | sed "s/,/\n/g" | wc -l`
CPUS=`echo $CPUMAP | sed "s/,/ /g"`

TSCCLK=`dmesg | grep "MHz processor" | sed "s/^.*Detected[ \t]\{1,\}\([\([0-9\.]\{1,\}\).*/\1/"`
STARTTIME=`cat /proc/uptime | sed "s/^\([^ \.]\{1,\}\).*/\1/"`
TOX=0
if [[ "$TSCCLK" == "" ]]; then
   TSCCLK=`cat /proc/cpuinfo | grep MHz | head -n1 | awk -F':' '{print $NF}' | sed 's/.//'`
fi
while test $TOX -gt -1
do

printf "%3s,\t%5s,\t%5s\n" "CPU" "MPERF" "APERF"
START=""
for CPU in $CPUS
do
STARTMPERF=`rdmsr -d -p $CPU 0xe7`
STARTAPERF=`rdmsr -d -p $CPU 0xe8`
START=`echo "$START$STARTMPERF-$STARTAPERF "`
done

sleep 1

APERF="0"
for CPU in $CPUS
do
TEMP=`echo $START | sed "s/ .*//"`
START=`echo $START | sed "s/^[^ ]\{1,\}//"`
STARTMPERF=`echo $TEMP | sed "s/-.*//"`
STARTAPERF=`echo $TEMP | sed "s/^.*-//"`
ENDMPERF=`rdmsr -d -p $CPU 0xe7`
ENDAPERF=`rdmsr -d -p $CPU 0xe8`
CCLK=`echo " ( $ENDAPERF - $STARTAPERF ) / ( $ENDMPERF - $STARTMPERF ) * $TSCCLK" | bc -l`
APERF=`echo "$APERF + $CCLK"`
printf "%3d,\t%5.0f,\t%5.0f\n" $CPU $TSCCLK $CCLK
done
AVECCLK=`echo "($APERF) / $NUMCORE" | bc -l`
printf "%3s,\t%5.0f,\t%5.0f\n\n" "AVE" $TSCCLK $AVECCLK

CURRTIME=`cat /proc/uptime | sed "s/^\([^ \.]\{1,\}\).*/\1/"`
TOX=`echo "$CURRTIME - $STARTTIME" | bc -l `
ipmitool sensor list | grep -i degree | grep -E '^CPU'
done
