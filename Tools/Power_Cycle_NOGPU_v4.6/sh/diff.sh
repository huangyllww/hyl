#!/bin/bash
product_name=`dmidecode -t system|grep -i "Product Name"|awk -F ":" '{print $2}'|sed 's/ //g'|head -1`
if [[ $product_name == "" ]];then
   product_name="SUT"
fi
Cur_Dir=$(cd "$(dirname "$0")";pwd)
LogAd="/var/log/$product_name/log"
ShAd="/var/log/$product_name/sh"
if [[ $1 == "NO" ]];then
   echo "no check,exit"
   exit
else
   cd $ShAd
   sh $ShAd/MachineCheck.sh > $LogAd/info_after.log
   cd -
fi
diff_cont=`cat $LogAd/info_before.log $LogAd/info_after.log | sort |uniq -u`
if [[ $diff_cont != "" ]]; then
   echo "##########################" >>$LogAd/errorinfo.log
   echo "the diff info:"|tee -a $LogAd/errorinfo.log
   date|tee -a $LogAd/errorinfo.log
   diff -s $LogAd/info_before.log $LogAd/info_after.log>tmp
   echo "the diff info in golden file is:"|tee -a $LogAd/errorinfo.log
   cat tmp|grep "<"|tee -a $LogAd/errorinfo.log
   echo "the diff info after reboot is:"|tee -a $LogAd/errorinfo.log
   cat tmp|grep ">"|tee -a $LogAd/errorinfo.log
   rm -rf $ShAd/tmp
   storcli /c0 show termlog > $ResultAd/RAID_termlog
   storcli /c0 show alilog > $ResultAd/RAID_alilog
   #cat $LogAd/test.log $LogAd/test_temp.log | sort |uniq -u|tee -a $LogAd/errorinfo.log
   exit 2
else
   exit 1
fi
if [ -f "$ShAd/raid-phy-error.flag" ]; then
	echo "exist phy error greater than 100 condition,please check $ShAd/raid-x-phyerror.log"
	exit 1
fi

