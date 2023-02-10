#!/bin/bash
product_name=`dmidecode -t system|grep -i "Product Name"|awk -F ":" '{print $2}'|sed 's/ //g'|head -1`
if [[ $product_name == "" ]];then
   product_name="SUT"
fi
Cur_Dir=$(cd "$(dirname "$0")";pwd)
LogAd="/var/log/$product_name/log"
ShAd="/var/log/$product_name/sh"
ResultAd="/var/log/$product_name/log/result"
System_SUSE=`less /etc/issue |grep SUSE |wc -l`
System_Redhat=`less /etc/issue |grep Red |wc -l`
System_CentOS=`less /etc/issue |grep Cent |wc -l`
System_Redhat7=`less /etc/redhat-release | grep "release 7" | wc -l`

if [ $System_SUSE -eq 1 ];then
echo "sh $ShAd/reboot.sh $1 $2 $3 $4 $5 $6 $7" >> /etc/bash.bashrc
elif [ $System_Redhat -eq 1 -o $System_CentOS -eq 1 -o $System_Redhat7 -eq 1 ];then
echo 'temp=`tty |grep tty1 |wc -l`' >> /root/.bash_profile
echo 'if [ $temp -eq 1 ];then' >> /root/.bash_profile
echo "sh $ShAd/reboot.sh $1 $2 $3 $4 $5 '$6' $7 $8 $9" >> /root/.bash_profile
echo "fi" >> /root/.bash_profile
fi
