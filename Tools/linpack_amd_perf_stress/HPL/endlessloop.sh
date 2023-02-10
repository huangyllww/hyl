#!/bin/sh
# define common functions
CP_ROOT_DIR=$(readlink -f $(dirname $0))
LOG_DIR=$CP_ROOT_DIR/log

if [ ! -d $LOG_DIR ]; then
	mkdir -p $LOG_DIR
else
	rm -rf $LOG_DIR/*
fi
monitor()
{
	starttime=$1
	currenttime=`date +%s`
	endtime=`echo "$starttime + $runtime * 3600"|bc -l |awk -F'.' '{print $1}'`
	while [ $endtime -ge $currenttime ]
	do	
		sleep 1
		currenttime=`date +%s`
	done
	killall -9 xhpl >/dev/null  2>&1
	pkill -9 xhpl	>/dev/null  2>&1
	killall -9 mpirun >/dev/null  2>&1
	pkill -9 mpirun >/dev/null  2>&1
}
help()
{
	echo "Usage :" 
	echo "   Specify run loops run as: sh endloops.sh -c 10"
	echo "   Specify runtime run as:sh endloops.sh -t 12,the unit is hour"
	echo "   Reboot run as: sh AutoRun.sh -i reboot"
	echo "Optional Parameters:"
	echo "   c: Specyfiy the the loops"
	echo "   t: Specify the runtime"
	echo "   if you do not specify the run type,default is -c 1"
  	exit 1
}
while getopts ":c:t:h" option 
do
    case $option in
        "c")
	    count=$OPTARG
	    ;;
        "t")
	    runtime=$OPTARG
	    ;;
	"*")
	    help
	    ;;
	"h")
	    help
	    ;;
    esac
done
if [[ $count == "" ]] && [[ "$runtime" == "" ]]; then
	count=1
fi

if [[ $count != "" ]] && [[ "$runtime" != "" ]]; then
	echo "You can only specify one type"
	help
fi

if [[ $count =~ "[0-9f]*" ]]; then
	echo "The count must be a number,not a $count"
	help
fi

#Configure the environment
ulimit -s unlimited
modprobe knem
if [ "$count" != "" ]; then 
	i=1
	while [ $i -le $count ]
	do
		sleep 1
		sync
		echo 3 > /proc/sys/vm/drop_caches

		numastat |tee ${CP_ROOT_DIR}/log/hpl_`hostname`_`date "+%Y%m%d-%H%M"`_numastat.out
		sh $CP_ROOT_DIR/run_hpl_node.sh 

		killall -9 xhpl
		killall -9 mpirun
		sleep 1
		let i=i+1
	done
fi

if [ "$runtime" != "" ]; then
	start_time=`date +%s`
	monitor $start_time &
	currenttime=`date +%s`
	difference=$(echo "($currenttime-$start_time)/3600" |bc -l)
	rs=`echo "($difference-$runtime)>0" | bc -l`
	# run linpack
	while [ $rs -eq 0 ]
	do
		sleep 1
		sync
		echo 3 > /proc/sys/vm/drop_caches

		numastat |tee ${CP_ROOT_DIR}/log/hpl_`hostname`_`date "+%Y%m%d-%H%M"`_numastat.out
		sh $CP_ROOT_DIR/run_hpl_node.sh 
		currenttime=`date +%s`
		difference=$(echo "($currenttime-$start_time)/3600" |bc -l)
		rs=`echo "($difference-$runtime)>0" | bc -l`
	done  
fi
