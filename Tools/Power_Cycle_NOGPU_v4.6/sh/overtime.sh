#!/bin/bash
product_name=`dmidecode -t system|grep -i "Product Name"|awk -F ":" '{print $2}'|sed 's/ //g'|head -1`
if [[ $product_name == "" ]];then
   product_name="SUT"
fi
Cur_Dir=$(cd "$(dirname "$0")";pwd)
LogAd="/var/log/$product_name/log"
ShAd="/var/log/$product_name/sh"
sed '1d' $LogAd/reboot.log > tmp1
Second=`less tmp1 |awk {'print $3'}`
Second_min=`less tmp1|sed -n '1p' |awk {'print $3'}`
Second_max=`less tmp1|sed -n '1p' |awk {'print $3'}`
    for Second_next in $Second
    do
          if [[ $Second_min -gt $Second_next ]];then
             Second_min=$Second_next
          elif [[ $Second_max -lt $Second_next ]];then
             Second_max=$Second_next
          fi
    done
overslow=`expr $Second_min \* 4`
if [[ $Second_max -gt $overslow ]];then
   Loop_temp=`less tmp1 |grep "$Second_max"|awk {'print $1'}`
   Second_temp=`less tmp1|grep "$Second_max"|awk {'print $3'}`
   echo "`date`:Overtime error Loop $Loop_temp used $Second_temp Seconds" >> $ResultAd/error.out
fi
rm -rf tmp1
