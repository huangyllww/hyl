#! /bin/bash
#
# autologin.sh
#

system_SLES11SP2=`less /etc/issue | grep "SUSE Linux Enterprise Server 11 SP2" | wc -l`
system_SLES11SP3=`less /etc/issue | grep "SUSE Linux Enterprise Server 11 SP3" | wc -l`
system_SLES12=`less /etc/issue | grep "SUSE Linux Enterprise Server 12" | wc -l`
system_Redhat5=`less /etc/issue | grep "Red Hat Enterprise Linux Server release 5" | wc -l` 
system_Redhat6=`less /etc/issue | grep "Red Hat Enterprise Linux Server release 6" | wc -l` 
system_Redhat7=`less /etc/redhat-release | grep "release 7" | wc -l` 
system_CentOS6=`less /etc/issue |grep "CentOS release 6" |wc -l`

if [[ ${system_SLES11SP3} == "1" || ${system_SLES11SP2} == "1" ]]; then
    	sed -i 's/id:5:initdefault:/id:3:initdefault:/' /etc/inittab
    	sed -i 's#1:2345:respawn:/sbin/mingetty --noclear tty1#1:2345:respawn:/sbin/mingetty --noclear --autologin root tty1#' /etc/inittab
    	exit
elif [[ ${system_SLES12} == "1" ]]; then
	ln -sf /usr/lib/systemd/system/multi-user.target /etc/systemd/system/default.target 
	sed -i 's#ExecStart=-/sbin/agetty --noclear %I $TERM#ExecStart=-/sbin/agetty --noclear --autologin root %I $TERM#' /usr/lib/systemd/system/getty@.service 
	exit
elif [[ ${system_Redhat5} == "1" ]]; then
    	sed -i 's/id:5:initdefault:/id:3:initdefault:/' /etc/inittab
    	sed -i 's#1:2345:respawn:/sbin/mingetty tty1#1:2345:respawn:/sbin/mingetty --noclear --autologin root tty1#' /etc/inittab
    	exit
elif [[ ${system_Redhat6} == "1" ]]; then
	sed -i 's/id:5:initdefault:/id:3:initdefault:/' /etc/inittab
    	sed -i 's#exec /sbin/mingetty#exec /sbin/mingetty --autologin root#' /etc/init/tty.conf
    	exit
elif [[ $system_CentOS6 == "1" ]]; then
    sed -i 's/id:5:initdefault:/id:3:initdefault:/' /etc/inittab
    sed -i 's#exec /sbin/mingetty#exec /sbin/mingetty --autologin root#' /etc/init/tty.conf
     exit
elif [[ ${system_Redhat7} == "1" ]]; then
        ln -sf /usr/lib/systemd/system/multi-user.target /etc/systemd/system/default.target 
        sed -i 's#ExecStart=-/sbin/agetty --noclear %I#ExecStart=-/sbin/agetty --noclear --autologin root %I#' /usr/lib/systemd/system/getty@.service 
        if [ -f "/etc/systemd/system/getty@tty1.service" ]; then
        	sed -i 's#ExecStart=-/sbin/agetty --noclear %I#ExecStart=-/sbin/agetty --noclear --autologin root %I#' /etc/systemd/system/getty@tty1.service 
	fi
        if [ -f "/etc/systemd/system/getty.target.wants/getty@tty1.service" ]; then
        	sed -i 's#ExecStart=-/sbin/agetty --noclear %I#ExecStart=-/sbin/agetty --noclear --autologin root %I#' /etc/systemd/system/getty.target.wants/getty@tty1.service 
	fi
        if [ -f "/usr/lib/systemd/system/serial-getty@.service " ]; then
	       sed -i 's#ExecStart=-/sbin/agetty --keep-baud 115200,38400,9600 %I $TERM#ExecStart=-/sbin/agetty --keep-baud 115200,38400,9600 %I $TERM --autologin root %I#' /usr/lib/systemd/system/serial-getty@.service 

	fi
        exit
fi 
