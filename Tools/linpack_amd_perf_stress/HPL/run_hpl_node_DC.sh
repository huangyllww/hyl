#!/bin/sh

CP_ROOT_DIR=$(cd "$(dirname "$0")";pwd)
Tool=$CP_ROOT_DIR/tool
MachineCheck=$Tool/MachineCheck
FRE_LOG=$CP_ROOT_DIR/freLog
HPL_LOG=$CP_ROOT_DIR/log

countLoops=10
threads=`lscpu | grep -i 'thread' | awk '{print $NF}'`
if [ $threads -ne 1 ]; then
        echo "SMT is not enable,please turn off SMT"
      #  exit 1
fi
        Ps=`sed -n '11p' HPL.dat`
        Qs=`sed -n '12p' HPL.dat`
        date=`date "+%Y%m%d-%H%M"`
        cpubindArray=($(numactl --show | awk -F':' '/^cpubind:/ {print $2}' | sed 's/[[:space:]]//'))
        membindArray=($(numactl --show | awk -F':' '/^membind:/ {print $2}' | sed 's/[[:space:]]//'))
        need_conf_file=${#membindArray[@]}

        if [ $need_conf_file -eq 1 ]; then

                Ps_new=`echo "$Ps" | sed 's/[0-9]/1/'`
                Qs_new=`echo "$Qs" | sed 's/[0-9]/1/'`
                sed -i "s/$Ps/$Ps_new/" HPL.dat
                sed -i "s/$Qs/$Qs_new/" HPL.dat
        fi
        if [ $need_conf_file -eq 2 ]; then

                Ps_new=`echo "$Ps" | sed 's/[0-9]/1/'`
                Qs_new=`echo "$Qs" | sed 's/[0-9]/2/'`
                sed -i "s/$Ps/$Ps_new/" HPL.dat
                sed -i "s/$Qs/$Qs_new/" HPL.dat
        fi
        if [ $need_conf_file -eq 3 ]; then

                Ps_new=`echo "$Ps" | sed 's/[0-9]/1/'`
                Qs_new=`echo "$Qs" | sed 's/[0-9]/3/'`
                sed -i "s/$Ps/$Ps_new/" HPL.dat
                sed -i "s/$Qs/$Qs_new/" HPL.dat
        fi
        if [ $need_conf_file -eq 4 ]; then

                Ps_new=`echo "$Ps" | sed 's/[0-9]/2/'`
                Qs_new=`echo "$Qs" | sed 's/[0-9]/2/'`
                sed -i "s/$Ps/$Ps_new/" HPL.dat
                sed -i "s/$Qs/$Qs_new/" HPL.dat
        fi
        if [ $need_conf_file -eq 5 ]; then

                Ps_new=`echo "$Ps" | sed 's/[0-9]/1/'`
                Qs_new=`echo "$Qs" | sed 's/[0-9]/5/'`
                sed -i "s/$Ps/$Ps_new/" HPL.dat
                sed -i "s/$Qs/$Qs_new/" HPL.dat
        fi

        if [ $need_conf_file -eq 6 ]; then

                Ps_new=`echo "$Ps" | sed 's/[0-9]/2/'`
                Qs_new=`echo "$Qs" | sed 's/[0-9]/3/'`
                sed -i "s/$Ps/$Ps_new/" HPL.dat
                sed -i "s/$Qs/$Qs_new/" HPL.dat
        fi
        if [ $need_conf_file -eq 7 ]; then

                Ps_new=`echo "$Ps" | sed 's/[0-9]/1/'`
                Qs_new=`echo "$Qs" | sed 's/[0-9]/7/'`
                sed -i "s/$Ps/$Ps_new/" HPL.dat
                sed -i "s/$Qs/$Qs_new/" HPL.dat
        fi
        if [ $need_conf_file -eq 8 ]; then

                Ps_new=`echo "$Ps" | sed 's/[0-9]/2/'`
                Qs_new=`echo "$Qs" | sed 's/[0-9]/4/'`
                sed -i "s/$Ps/$Ps_new/" HPL.dat
                sed -i "s/$Qs/$Qs_new/" HPL.dat
        fi
	#178176       Ns
	memtotal=`cat /proc/meminfo | grep -i 'memtotal' | awk '{print $2}'`
	Ns=`echo "scale=0;sqrt($memtotal*1024/8*0.8)" | bc -l | awk -F'.' '{print $1}'`
	Ns_origin=`sed -n '/[0-9]*[[:space:]]Ns/p' HPL.dat`
	Ns_new=`echo "$Ns_origin" | sed "s/[0-9]*/$Ns/"`
	sed -i "s/$Ns_origin/$Ns_new/" HPL.dat
#	echo "$Ns_new"
function getconfigfile()
{
	if [ "$threads" == "1" ]; then
        	index=$1
	        seqarator=`lscpu | sed -n "/NUMA node$index/p" | awk -F':' '{print $2}' | sed 's/[[:space:]]//g' | sed 's/[0-9]*\(-\)[0-9]*/\1/' | sed 's/[0-9]*\(,\)[0-9]*/\1/'`
        	start=`lscpu | sed -n "/NUMA node$index/p" | awk -F':' '{print $2}' | sed 's/[[:space:]]//g' | sed "s/\([0-9]*\)$seqarator[0-9]*/\1/"`
	        end=`lscpu | sed -n "/NUMA node$index/p" | awk -F':' '{print $2}' | sed 's/[[:space:]]//g' | sed "s/[0-9]*$seqarator\([0-9]*\)/\1/"`
        	GOMP_CPU_AFFINITY=()
	        local i=0
        	for((i=$start;i<=$end;i++))
	        do
        	        GOMP_CPU_AFFINITY[$i]=$i
	        done
  #      echo "${GOMP_CPU_AFFINITY[@]}"
        	OMP_NUM_THREADS=`sed -n '/OMP_NUM_THREADS/p' xhpl_node"$index".sh`
	        OMP_NUM_THREADS=`echo "$OMP_NUM_THREADS" | sed 's/.$//'`
        	OMP_NUM_THREADS=$OMP_NUM_THREADS"${#GOMP_CPU_AFFINITY[@]}"

	        sed -i '/OMP_NUM_THREADS/d'  xhpl_node"$index".sh
        	sed -i "2i\\$OMP_NUM_THREADS"  xhpl_node"$index".sh

	        GOMP_CPU_temp=`sed -n '/GOMP_CPU_AFFINITY/p'  xhpl_node"$index".sh`
        	GOMP_CPU_temp=`echo "$GOMP_CPU_temp" | awk -F'=' '{print $1}'`
	        GOMP_CPU_temp=$GOMP_CPU_temp"=\"${GOMP_CPU_AFFINITY[@]}\""
 #      	 echo "$GOMP_CPU_temp"
	        sed -i '/GOMP_CPU_AFFINITY/d'  xhpl_node"$index".sh
        	sed -i "3i\\$GOMP_CPU_temp"  xhpl_node"$index".sh

	        BLIS_JC_NT=`sed -n '/BLIS_JC_NT/p'  xhpl_node"$index".sh`
        	BLIS_JC_NT=`echo "$BLIS_JC_NT" | sed 's/.$//'`
		die_cores=${#GOMP_CPU_AFFINITY[@]}
        	BLIS_JC_NT_num=`expr $die_cores / 2`
	        BLIS_JC_NT=$BLIS_JC_NT"$BLIS_JC_NT_num"

        	sed -i '/BLIS_JC_NT/d'  xhpl_node"$index".sh
	        sed -i "8 i\\$BLIS_JC_NT"  xhpl_node"$index".sh

        	physcpubind_origi=`awk '/cpubind/ {print $0}' xhpl_node"$index".sh| sed 's/.*--\(.*cpubind=.*\).--.*/\1/'`
		cpubindarray=`echo "${GOMP_CPU_AFFINITY[@]}" | sed 's/[[:space:]]/,/g'`
        	physcpubind_new="physcpubind="${cpubindarray}
		sed -i "s/$physcpubind_origi/$physcpubind_new/" xhpl_node"$index".sh
#        echo "$physcpubind_new"
	else
        	GOMP_CPU_AFFINITY=()
	        local i=0
		local j=0
	        index=$1
        	coreList=`lscpu | sed -n "/NUMA node$index/p" | awk -F':' '{print $2}' | sed 's/[[:space:]]//g' | sed 's/,/ /'`
		for coreTemp in ${coreList}
		do
		        seqarator=`echo "$coreTemp" | sed 's/[0-9]*\(-\)[0-9]*/\1/' | sed 's/[0-9]*\(,\)[0-9]*/\1/'`
		        start=`echo "$coreTemp" | sed "s/\([0-9]*\)$seqarator[0-9]*/\1/"`
		        end=`echo "$coreTemp" | sed "s/[0-9]*$seqarator\([0-9]*\)/\1/"`
		        for((i=$start;i<=$end;i++))
		        do
                		GOMP_CPU_AFFINITY[$j]=$i
				let j=j+1
		        done
		done
		echo "${GOMP_CPU_AFFINITY[@]}"
		OMP_NUM_THREADS=`sed -n '/OMP_NUM_THREADS/p' xhpl_node"$index".sh`
		        OMP_NUM_THREADS=`echo "$OMP_NUM_THREADS" | sed 's/.$//'`
		        OMP_NUM_THREADS=$OMP_NUM_THREADS"${#GOMP_CPU_AFFINITY[@]}"

		        sed -i '/OMP_NUM_THREADS/d'  xhpl_node"$index".sh
		        sed -i "2i\\$OMP_NUM_THREADS"  xhpl_node"$index".sh

		        GOMP_CPU_temp=`sed -n '/GOMP_CPU_AFFINITY/p'  xhpl_node"$index".sh`
		        GOMP_CPU_temp=`echo "$GOMP_CPU_temp" | awk -F'=' '{print $1}'`
		        GOMP_CPU_temp=$GOMP_CPU_temp"=\"${GOMP_CPU_AFFINITY[@]}\""
 #       echo "$GOMP_CPU_temp"
		        sed -i '/GOMP_CPU_AFFINITY/d'  xhpl_node"$index".sh
		        sed -i "3i\\$GOMP_CPU_temp"  xhpl_node"$index".sh

		        BLIS_JC_NT=`sed -n '/BLIS_JC_NT/p'  xhpl_node"$index".sh`
		        BLIS_JC_NT=`echo "$BLIS_JC_NT" | sed 's/.$//'`
			die_cores=${#GOMP_CPU_AFFINITY[@]}
		        BLIS_JC_NT_num=`expr $die_cores / 2`
		        BLIS_JC_NT=$BLIS_JC_NT"$BLIS_JC_NT_num"

		        sed -i '/BLIS_JC_NT/d'  xhpl_node"$index".sh
		        sed -i "8 i\\$BLIS_JC_NT"  xhpl_node"$index".sh	

		        physcpubind_origi=`awk '/cpubind/ {print $0}' xhpl_node"$index".sh| sed 's/.*--\(.*cpubind=.*\).--.*/\1/'`
			cpubindarray=`echo "${GOMP_CPU_AFFINITY[@]}" | sed 's/[[:space:]]/,/g'`
		        physcpubind_new="physcpubind="${cpubindarray}
			sed -i "s/$physcpubind_origi/$physcpubind_new/" xhpl_node"$index".sh
		        echo "$physcpubind_new"
	fi

}
function insertAutoRun()
{
        echo 'temp=`tty |grep tty1 |wc -l`' >> /root/.bash_profile
        echo 'if [ $temp -eq 1 ];then' >> /root/.bash_profile
        echo "cd $CP_ROOT_DIR" >> /root/.bash_profile
        echo "sh $CP_ROOT_DIR/$0" >> /root/.bash_profile
        echo "fi" >> /root/.bash_profile
}

#recover the system initial status
function recover()
{
        #recover initial#
        sed -i '/tty/d' /root/.bash_profile
        sed -i '/if/d' /root/.bash_profile
        sed -i '/cd/d' /root/.bash_profile
        sed -i '/sh/d' /root/.bash_profile
        sed -i '/fi/d' /root/.bash_profile
        sed -i '3 i\ . ~/.bashrc \\' /root/.bash_profile
}
function init()
{
        if [[ ! -f ${CP_ROOT_DIR}/LOOPS ]]; then
		if [ "$loops" == "" ]; then
			loops=$countLoops
		fi
		echo $loops >${CP_ROOT_DIR}/loops
            	echo "Loop time: 1"
        	echo "1" > ${CP_ROOT_DIR}/LOOPS
	        rm -f $FRE_LOG/* 
        	rm -f $HPL_LOG/HPL_"$hostname"*\.out
        	rm -f $HPL_LOG/*.log
            	info_before=0
		if [ ! -f $MachineCheck/install.flag ]; then
			sh $MachineCheck/install.sh
			touch $MachineCheck/install.flag
		fi
		cd $MachineCheck
                sh $MachineCheck/MachineCheck.sh | tee ${CP_ROOT_DIR}/info_1.log
		cd -
                curlooptime=`cat ${CP_ROOT_DIR}/LOOPS | sort -n -ur | head -n 1`
        else
		loops=`cat ${CP_ROOT_DIR}/loops`
                curlooptime=`cat ${CP_ROOT_DIR}/LOOPS | sort -n -ur | head -n 1`
                info_before=$curlooptime
                curlooptime=`expr $curlooptime + 1`
                echo "Loop time: $curlooptime"
        	echo $curlooptime >> ${CP_ROOT_DIR}/LOOPS
		cd $MachineCheck
                sh $MachineCheck/MachineCheck.sh | tee ${CP_ROOT_DIR}/info_"$curlooptime".log
		cd -
        fi
        if [ -f ${CP_ROOT_DIR}/info_"$info_before".log ]; then
                result=`diff ${CP_ROOT_DIR}/info_"$curlooptime".log ${CP_ROOT_DIR}/info_"$info_before".log`
                if [ -n "$result" ]; then
                        echo "the $info_before loop machine message not equal $curlooptime" | tee -a ${CP_ROOT_DIR}/error.log
			echo "$result" >>${CP_ROOT_DIR}/error.log
                #       exit 1
                else
                        rm -f ${CP_ROOT_DIR}/info_"$info_before".log >/dev/null
                fi
        fi
	if [ $curlooptime -gt $loops ]; then
		echo "Linpack + DC complete"
		exit 0
	fi
}
# kill twice
sleep 1
killall -9 xhpl   >& /dev/null
killall -9 mpirun >& /dev/null

ulimit -s unlimited
ulimit -l unlimited
echo 3 > /proc/sys/vm/drop_caches

recover

init
hostname=`hostname`
log_name="HPL_`hostname`_"$curlooptime".out"
#run get cpu freq real-time
sh $Tool/get_APERF_FREQ.sh >> $FRE_LOG/cpu_freq_monitor_$curlooptime.log &
index=0
cp -f appfile_node appfile_node.bak
for i in ${membindArray[@]}
do
	if [ ${cpubindArray[$index]} -eq ${membindArray[$index]} ]; then
                getconfigfile $i
                config_file=`awk "/-np 1 .\/xhpl_node"$i".sh/" appfile_node`
                echo "$config_file" >> temp
        fi
	let index=index+1
done
cat temp > appfile_node
rm -f temp >/dev/null
mpi_options="--allow-run-as-root --mca mpi_leave_pinned 1  --bind-to none --report-bindings --mca btl self,vader"
sudo /opt/openmpi/bin/mpirun $mpi_options -app ./appfile_node | tee -a $HPL_LOG/$log_name
cp -f appfile_node.bak appfile_node

rs=`sh $CP_ROOT_DIR/get_results.sh "$log_name" "$curlooptime"`
efficient=`echo "$rs" | grep -i average | awk '{print $NF}' | awk -F'.' '{print $1}'`
echo "$rs" | tee -a $HPL_LOG/efficiency.log

if [ $curlooptime -lt $loops ];then
	echo "processor: DC(ipmitool power cycle)"
	echo "you have 10s to abort"
	sleep 10
	insertAutoRun
#        ipmitool power cycle
	reboot
else
        if [ $occur_error_flag -eq 1 ]; then
                echo "Occur error between $info_before loop and $curlooptime loop"
                exit 1
        else
                echo "The End"
                exit 0
        fi
fi
#/opt/openmpi/bin/mpirun --allow-run-as-root --bind-to none -app ./appfile_node
