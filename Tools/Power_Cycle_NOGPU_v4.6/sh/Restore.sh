#!/bin/bash
system_SUSE=`less /etc/issue |grep SUSE |wc -l`
system_Redhat5=`less /etc/issue |grep "Red Hat Enterprise Linux Server release 5" |wc -l` 
system_Redhat6=`less /etc/issue |grep "Red Hat Enterprise Linux Server release 6" |wc -l`
system_CentOS6=`less /etc/issue |grep "CentOS release 6" |wc -l`
System_Redhat7=`less /etc/redhat-release | grep "release 7" | wc -l`

if [[ $system_SUSE == "1" ]]; then
     rm -f /etc/bash.bashrc
     cp -f /etc/bash.bashrc.bak /etc/bash.bashrc
     rm -f /etc/bash.bashrc.bak
     rm -f /etc/inittab
     cp -f /etc/inittab.bak /etc/inittab
     rm -f /etc/inittab.bak
    exit
elif [[ $system_Redhat5 == "1" ]]; then
     
     rm -f /etc/inittab
     cp -f /etc/inittab.bak /etc/inittab
     rm -f /etc/inittab.bak
     rm -f /root/.bash_profile
     cp -f /root/.bash_profile.bak /root/.bash_profile
     rm -f /root/.bash_profile.bak
     exit
elif [[ $system_Redhat6 == "1" ]];then
     rm -f /etc/inittab
     cp -f /etc/inittab.bak /etc/inittab
     rm -f /etc/inittab.bak
     rm -f /root/.bash_profile
     cp -f /root/.bash_profile.bak /root/.bash_profile
     rm -f /root/.bash_profile.bak
     rm -f /etc/init/tty.conf
     cp -f /etc/init/tty.conf.bak /etc/init/tty.conf
     rm -f /etc/init/tty.conf.bak
     exit

elif [[ $System_Redhat7 == "1" ]];then
     rm -f /etc/inittab
     cp -f /etc/inittab.bak /etc/inittab
     rm -f /etc/inittab.bak
     rm -f /root/.bash_profile
     cp -f /root/.bash_profile.bak /root/.bash_profile
     rm -f /root/.bash_profile.bak
     exit
elif [[ $system_CentOS6 == "1" ]];then
     rm -f /etc/inittab
     cp -f /etc/inittab.bak /etc/inittab
     rm -f /etc/inittab.bak
     rm -f /root/.bash_profile
     cp -f /root/.bash_profile.bak /root/.bash_profile
     rm -f /root/.bash_profile.bak
     rm -f /etc/init/tty.conf
     cp -f /etc/init/tty.conf.bak /etc/init/tty.conf
     rm -f /etc/init/tty.conf.bak
     exit

fi

