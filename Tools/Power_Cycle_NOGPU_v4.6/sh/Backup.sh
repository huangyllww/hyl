#!/bin/bash
system_SUSE=`less /etc/issue |grep SUSE |wc -l`
system_Redhat5=`less /etc/issue |grep "Red Hat Enterprise Linux Server release 5" |wc -l`
system_Redhat6=`less /etc/issue |grep "Red Hat Enterprise Linux Server release 6" |wc -l`
system_CentOS6=`less /etc/issue |grep -E "CentOS 6|CentOS release 6" |wc -l` 
System_Redhat7=`less /etc/redhat-release | grep "release 7" | wc -l`

if [[ $system_SUSE == "1" ]]; then
    cp /etc/bash.bashrc /etc/bash.bashrc.bak
    cp /etc/inittab   /etc/inittab.bak
    exit
elif [[ $system_Redhat5 == "1" ]]; then
    cp /root/.bash_profile  /root/.bash_profile.bak
    cp /etc/inittab /etc/inittab.bak
    exit
elif [[ $system_Redhat6 == "1" ]];then
    cp /root/.bash_profile /root/.bash_profile.bak
    cp /etc/inittab /etc/inittab.bak
    cp /etc/init/tty.conf /etc/init/tty.conf.bak
    exit
elif [[ $System_Redhat7 == "1" ]];then
    cp /root/.bash_profile /root/.bash_profile.bak
    cp /etc/inittab /etc/inittab.bak
    exit
elif [[ $system_CentOS6 == "1" ]];then
    cp /root/.bash_profile /root/.bash_profile.bak
    cp /etc/inittab /etc/inittab.bak
    cp /etc/init/tty.conf /etc/init/tty.conf.bak
    exit

fi
