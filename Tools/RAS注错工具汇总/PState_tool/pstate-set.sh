#!/bin/bash
#////////////////////////////////////////////////////////////////////////////
# @copyright  Copyright(c) 2018
# @copyright  Higon Platform, FW Group
# @copyright  All Right Reserved.
#
# function: pstate tool
# usage: ./pstate-set.sh [cpu] [psate_level]
# 
# @author     chenyulong<chenyulong@higon.com>
# @date       2018-03-02
#
# revision history:
#  $Id$
#////////////////////////////////////////////////////////////////////////////

function usage()
{
    echo "usage: $0 [cpu] [psate_level]"
    echo "cpu:         0 for cpu0, 1 for cpu1 ..., -1 for all cpu" 
    echo "psate_level: 0 for P0, 1 for P1 ..."
    echo ""
    echo "aviliable cpu: 0 - $nCpu"
    echo "aviliable pstate:"
    for((i=0;i<$num;i++))
    do
        echo "P$i ${pstate_list[$i]}"
    done
    exit 0
}

function setcpufreq()
{
    cpu=$1
    freq=$2
    if [ "$2" != "ondemand" ]; then
    	echo userspace > /sys/devices/system/cpu/cpu$cpu/cpufreq/scaling_governor
    	echo $freq     > /sys/devices/system/cpu/cpu$cpu/cpufreq/scaling_setspeed
    else 
	echo ondemand > /sys/devices/system/cpu/cpu$cpu/cpufreq/scaling_governor
    fi
}

pstate_list=($(cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_available_frequencies))
num=${#pstate_list[@]}
nCpu=$(lscpu|awk '/^CPU\(s\)/{print $2}')

if [ "$2" == "" ]; then
    usage
fi

if [ "$1" -lt "-1" ]; then
    usage
fi

if [ "$1" -ge "$nCpu" ]; then
    usage
fi

if [ "$2" != "ondemand" ]; then

	if [ "$2" -lt "0" ]; then
	    usage
	fi
	
	if [ "$2" -ge "$num" ]; then
	    usage
	fi

fi

cpu=$1
freq=${pstate_list[$2]} 

if [ "$cpu" == "-1" ]
then
    for((cpu=0;cpu<$nCpu;cpu++))
    do
        setcpufreq $cpu $freq
    done
else
    setcpufreq $cpu $freq
fi
