#!/bin/bash
product_name=`dmidecode -t system|grep -i "Product Name"|awk -F ":" '{print $2}'|sed 's/ //g'|head -1`
if [[ $product_name == "" ]];then
   product_name="SUT"
fi

Cur_Dir=$(cd "$(dirname "$0")";pwd)
LogAd="/var/log/$product_name/log"
ShAd="/var/log/$product_name/sh"


temp=`ls /etc |grep inittab.bak |wc -l`
if [[ $temp == "1" ]]; then 
   sh $ShAd/Restore.sh
fi
temp=`ls /etc |grep bash.bashrc.bak|wc -l`
if [[ $temp == "1" ]]; then
   sh $ShAd/Restore.sh
fi
temp=`ls /root |grep .bash_profile.bak|wc -l`
if [[ $temp == "1" ]]; then
   sh $ShAd/Restore.sh
fi
blackSmallBox=false
if [ ${#} -eq 6 ]; then
    blackSmallBox=true
    delay=$1
    object=$2
    stop_flag=$3
    loop=$4
    pdu_ip=$5
    safe="$6"
else
    delay=$1
    object=$2
    stop_flag=$3
    loop=$4
    Model=$5
    port="$6"
    ac_server=$7
    acserverport=$8
    safe=$9
fi
function collect_log()
{
   echo "**********" `date +%m-%d" "%H:%M:%S` "Collecting logs **********"
   mce_log="/var/log/mcelog"
   messages_log="/var/log/messages"
   dmesg_log="/var/log/dmesg"
   if [ -f $mce_log ]; then
   	cat $mce_log > $LogAd/mce_log
   fi
   cat $messages_log > $LogAd/messages_log
   cat $dmesg_log > $LogAd/dmesg_log

   service ipmi start
   ipmitool sel list > $LogAd/bmc_log
}

function getCoolPolicy()
{
bmc_ip=`ipmitool lan print|grep -i 'IP Address          '|awk -F ":" '{print $2}'`
login_info=`curl -X POST -d "username=admin&password=admin" http://$bmc_ip/api/session -c cookie 2>/dev/null`
verifyCode=`echo $login_info|awk -F ":" '{print $10}'|awk '{print $1}'`
var_0=`echo ${verifyCode:1:8}`
var_1=X-CSRFTOKEN:$var_0
expect_info=`curl -X GET -H $var_1 http://$bmc_ip/api/s_cool-policy -b cookie 2>/dev/null`
resus=`echo $expect_info|grep -i 'cool_policy'|awk -F '{' '{print $2}'|awk -F ',' '{print $1}'`
echo $resus
ext=`curl -X DELETE -H $var_1 http://$bmc_ip/api/session -b cookie 2>/dev/null`
}


function ConnectACServer()
{
   while true
   do
      recv=`$ShAd/client.o $ac_server "sh /root/test$Model.sh '$port' 70 30" "$acserverport" `
      if [[ $recv =~ "Received:" ]];then
         break
      else
         sleep 1
         continue
      fi
   done
}

Loop=`sed -n '$p' $LogAd/reboot.log|awk {'print $1'}`
if [ -z $Loop ];then
   Loop=0
fi
Second=`date +%s`
Second_pre=`sed -n '$p' $LogAd/reboot.log|awk {'print $2'}`
let Second_count=$Second-$Second_pre
awk 'NR>1{print a}{a=$0}END{print gensub(/$/,"'$Second_count'",$0)}' $LogAd/reboot.log >$LogAd/reboot1.log
sleep 2
mv $LogAd/reboot1.log $LogAd/reboot.log
overtime_temp=`sed -n '$p' $LogAd/reboot.log|awk {'print $3'}`
if [ $overtime_temp -gt 0 ];then
   sh $ShAd/overtime.sh
fi
cd $ShAd >/dev/null
sh $ShAd/MachineInfo.sh $Loop $LogAd > $LogAd/info.all
cd - >/dev/null

#Get RAID card temp
if [ -f $LogAd/ROC_Tem.log ]; then
        ROC_Temp=`awk 'NF{a=$0}END{print a}' $LogAd/ROC_Tem.log|awk '{print $1}'`
        if [[ ! -n "$ROC_Temp" ]];then
           echo "Cant' t Get RAID Controller's temp"
        else
           raid_num=`storcli show ctrlcount | awk -F'=' '/Controller Count/ {print $2}' | sed 's/[[:space:]]//g'`
           for((i=0;i<$raid_num;i++))
           do
              if [[ $ROC_Temp -gt 105 ]];then
                 storcli /c$i show termlog > $LogAd/RAID_termlog
                 storcli /c$i show alilog > $LogAd/RAID_alilog
                 echo "The Temperature of RAID Controller is higher than 105, so exit!"
                 exit 1
              fi
           done
        fi
fi

#diff info
sh $ShAd/diff.sh $object
if [ $? -eq 2 ]; then
   echo "There is something wrong with your CPU HDD or Mem at Loop $Loop" >>$LogAd/error.out
   if [ $stop_flag == "STOP" ];then
      echo "stop_flag is STOP,so exit"
      collect_log
      exit
   elif [ $stop_flag == "NON-STOP" ];then
      continue
   else
      echo "Unsupport stop flag, and it shoule be STOP or NON-STOP, exit..."
      exit
   fi
fi
###
while [[ $Loop -le $loop ]]
do
   let Loop=$Loop+1
   echo "Current Loop is $Loop"
   echo $Loop $Second "   ">> $LogAd/reboot.log
   sleep 2
   echo "The system will reboot in $1 second"
   sleep 1
   echo "Press Ctrl+c to stop running"
   sleep $delay
   sh $ShAd/Backup.sh
   sh $ShAd/autologin.sh
   if [ "$blackSmallBox" == "true" ]; then
      Retry_Times=3
      sh $ShAd/autoopen.sh $delay $object $stop_flag $loop $pdu_ip $safe
      #####################Retry $Retry_Times try to shutdown PDU ######################
      count=1
      python $ShAd/pwrctl.py $pdu_ip 11 70,70
      if [ $? -ne 0 ]; then
         while [ $count -le $Retry_Times ]
         do
            python $ShAd/pwrctl.py $pdu_ip 11 70,70
            if [ $? -eq 0 ]; then
               break
            fi  
            let count=count+1
         done  
     fi  
     if [ $count -eq 3 ]; then
        echo "Retry three times, still can not normally shut down the PDU"
        exit 1    
     fi  
   else
       sh $ShAd/autoopen.sh $delay $object $stop_flag $loop $Model "$port" $ac_server "$acserverport" $safe
       ConnectACServer
   fi
   sleep 20
   if [ $safe == "YES" ];then
       init 0
   else
       echo "The test type is unsafe model"
       continue
   fi
   echo system reboot here
   sleep 365
done
collect_log

