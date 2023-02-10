#! /bin/bash
#Version
#
echo "-----------------------------------------"
echo "Sugon Power Cycle test tool -- Customize"
echo "-----------------------------------------"
echo ""


###############Create Run Dir###############
product_name=`dmidecode -t system|grep -i "Product Name"|awk -F ":" '{print $2}'|sed 's/ //g'|head -1`
if [[ $product_name == "" ]];then
   product_name="SUT"
fi
Cur_Dir=$(cd "$(dirname "$0")";pwd)
LogAd="/var/log/$product_name/log"
ShAd="/var/log/$product_name/sh"
system_Redhat=`less /etc/redhat-release |grep Red |wc -l` 
if [[ $system_Redhat == "1" ]]; then
    service sendmail stop 2>/dev/null
    chkconfig --levels 12345 sendmail off 2>/dev/null
fi

mce_log=/var/log/mcelog
messages_log=/var/log/messages
dmesg_log=/var/log/dmesg

rm -rf /var/log/$product_name
mkdir -p $ShAd 2>&1>/dev/null
mkdir -p $LogAd 2>&1>/dev/null

cd $ShAd >/dev/null
########### Clear System Log ###############

if [ -f $mce_log ]; then
	echo "">$mce_log
fi
echo "">$messages_log
echo "">$dmesg_log
ipmitool sel clear

########### Prepare Log File###############
rm -rf $Cur_Dir/sh/reboot.sh

rm -rf $Cur_Dir/sh/*.txt
rm -rf $Cur_Dir/sh/*.log
rm -rf $Cur_Dir/sh/*.flag

cp -rf $Cur_Dir/sh/* $ShAd
echo "Loop   time   reboot_time" > $LogAd/reboot.log


function findAvailableEth()
{
    eths=($(ip addr | grep "BROADCAST" | grep -v -E "virbr|docker" | awk -F':' '{print $2}' | sed 's/[[:space:]]//g'))

    local findEth=""
    for i in ${eths[@]}
    do  
        #echo "$i"
        linkStatus=`ethtool "$i" | grep "Link" | awk -F':' '{print $NF}' | sed 's/[[:space:]]//g' | tr 'a-z' 'A-Z'`
        existIP=`ifconfig "$i" | grep "inet" | grep -v "inet6"`    
        if [[ "$linkStatus" == "NO" ]] || [[ "$existIP" != "" ]];then
            continue
        else
            if [[ "$existIP" == "" ]]; then
                findEth="$i"
                break
            else
                continue
            fi  
        fi  
    done
    return "$findEth"
}

function setStaticEth()
{
    findEth="$1"
    echo "find eth : $findEth"
    findEth="ifcfg-$findEth"
    findEthBak="$findEth".bak
    ethFilePath="/etc/sysconfig/network-scripts/"
    ethFile="$ethFilePath$findEth"
    ethFileBak="$ethFilePath$findEthBak"

    #backup
    cp -f "$ethFile" "$ethFileBak"

    #set static
    ipaddr="IPADDR=192.168.0.100"
    netmask="NETMASK=255.255.255.0"
    sed -i 's/dhcp/static/' "$ethFile"
    sed -i 's/ONBOOT=no/NOBOOT=yes/' "$ethFile"
    echo "$ipaddr" >> "$ethFile"
    echo "$netmask" >> "$ethFile"
}

function recoverEth()
{
    findEth="$1"
    echo "find eth : $findEth"
    findEth="ifcfg-$findEth"
    findEthBak="$findEth".bak
    ethFilePath="/etc/sysconfig/network-scripts/"
    ethFile="$ethFilePath$findEth"
    ethFileBak="$ethFilePath$findEthBak"

    # recover eth
    mv -f "$ethFileBak" "$ethFile"
    if [ $? -eq 0 ]; then
        return 0
    else
        return 1
    fi
}

function InfoCheck(){
   if [[ $check == "NO" ]];then
      echo "Don't collect any info both HW and SW!"
   else
      cd $ShAd >/dev/null
      sh $ShAd/MachineCheck.sh|tee $LogAd/info_before.log
      if [ -f "$ShAd/raid-phy-error.flag" ]; then
	 echo "exist phy error greater than 100 condition,please check $ShAd/raid-x-phyerror.log"
	 exit 1
      fi
      if [ -f "$ShAd/upi_speed_illegal.flag" ]; then
	 echo "UPI Speed is slow,and exit now..."
	 exit 1
      fi
      cd -
:<<!
      read -n1 -p "The summary of machine whether match the reality condition......:(yY/nN)" resp
      if [[ $resp == "N" ]] || [[ $resp == "n" ]]; then
          exit 1
      fi
      if [[ $resp != "N" ]] && [[ $resp != "n" ]] && [[ $resp != "Y" ]] && [[ $resp != "y" ]]; then
          echo "Wrong input parameter: please input yY/nN"
          exit 1
      fi
!
      sleep 5
   fi
}

function UPICheck(){
   lspci|grep -i "205b">tmp
   if [ -s tmp ];then
      linenum=`awk '{print NR}' tmp|tail -n1`
      if [[ $linenum -eq 0 ]];then
         echo ' Set "EV DFX Feature" enable in BIOS setting, otherwise UPI can not be checked!'
         sleep 5
      fi
   fi
   rm -rf tmp
}

function help(){
   echo "Usage :" 
   echo "   DC run as:     sh AutoRun.sh -i dc"
   echo "   AC run as:     sh AutoRun.sh -i ac -p <0|0 1 2|...> -s <10.0.21.1|...> -m PDU1 -a 8801"
   echo "   AC run as black small box:     sh AutoRun.sh -i ac -m single -g 192.168.0.100 -j 192.168.0.200"
   echo "   Reboot run as: sh AutoRun.sh -i reboot"
   echo "Optional Parameters:"
   echo "   DC: -m <RTC|UTC>: the default value is UTC"
   echo "   AC: -m <PDU|IPS|APC>: the default value is PDU"
   echo "   -c <YES|NO>: If check the info, and the default is YES"
   echo "   -b <YES|NO>: If bmc cold reset or no, the default value is NO"
   echo "   -f <STOP|NON-STOP>: when diff occurs it will stop or not,the default is stop"
   echo "   -d <20|...>: S0 delay time,the default value is 10s"
   echo "   -w <60|...>: S5 delay time, the default value is 120s"
   echo "   -l <500|...>: the loops, and the default value is 1000"
   echo "   -a ac server port which the port that server1(1~7) listens on."
   echo "   -e default is safe=yes,means first shut down and then power off,else direct to poweroff"
   echo "   -g system static ip which connect to black small box,default is 192.168.0.100."
   echo "   -j black small box ip which connect to system,default is 192.168.0.200"
   exit
}

###main###
UPICheck
while getopts ":i:c:f:d:m:p:s:w:l:b:a:j:g:" option
do
   case $option in
      i)
         item=$OPTARG
         item=`echo $item|tr '[a-z]' '[A-Z]'`;;    ##DC or reboot or AC
      c)
         check=$OPTARG
         check=`echo $check|tr '[a-z]' '[A-Z]'`;;  ##if check info
      b) bmc_reset=$OPTARG
         bmc_reset=`echo $bmc_reset|tr '[a-z]' '[A-Z]'`;; ##if bmc cold reset
      f)
         flag=$OPTARG
         flag=`echo $flag|tr '[a-z]' '[A-Z]'`;;   ##stop or continue
      d)
         delay=$OPTARG;;            ##Delay time, the wait time befor reboot
      m)
         mode=$OPTARG
         mode=`echo $mode|tr '[a-z]' '[A-Z]'`;;  ##for DC, UTC or RTC;for AC, APC or PDU SINGLE
      w)
         wait=$OPTARG;;  ##shut down time for DC
      p)
         port=$OPTARG;;              ##AC port
      s)
         server_ip=$OPTARG;;         ##AC server ip
      l)
         loop=$OPTARG;;              ##LOOPS
      a)
         acserverport=$OPTARG;;              ##ac server port
      e) 
         safe=$OPTARG
         safe=`echo $item|tr '[a-z]' '[A-Z]'`;; ##AC safe shutdown,default "YES"
      g) 
         sysStaticIP=$OPTARG;;    ## system static ip you specify which connect to black small PDU
      j) 
         blackBoxStaticIP=$OPTARG;;    ## black small box static ip you configured which connect to system
      *)
         help;;
   esac
done
if [[ -z "$item" ]];then
   echo "Pls input the test item, for example: -i DC,exit now."
   help
fi
if [[ -z "$delay" ]];then
   delay=10                   ##the default time of delay before reboot is 10s
else
   expr $delay "+" 10 &> /dev/null 
   if [ $? -ne 0 ];then 
      echo "the input S0 delay time isn't a number,exit" 
      exit
   fi 
fi
if [[ -z "$check" ]];then
   check="YES"                ##check all by default
fi
if [[ -z "$bmc_reset" ]];then
   bmc_reset="NO"
fi
if [[ -z "$flag" ]];then
   flag="STOP"                ##stop when the diff occur by default
fi
if [[ -z "$loop" ]];then
   loop=1000                      ##the default of runing time is 12h
else
   expr $loop "+" 10 &> /dev/null
   if [ $? -ne 0 ];then
      echo "the input loop isn't a number,exit" 
      exit
   fi 
fi
if [[ -z "$safe" ]];then
    safe="YES"
fi 

###############Install hdparm tool###############
echo "Install tools"
sh $ShAd/install.sh >$ShAd/install_error.log 2>&1
if [ $? -ne 0 ]; then
   echo "install tools occur error,please check,more detail message in $ShAd/install_error.log"
   exit 1
fi
###############Install hdparm tool###############

echo "0 `date +%s` " >> $LogAd/reboot.log
if [[ $item == "REBOOT" ]];then
   echo "Reboot test start..."
   cp $Cur_Dir/sh/mode/reboot_reboot.sh $ShAd/reboot.sh
   ############Info Collect####################
   InfoCheck
   cd $ShAd >/dev/null
   sh $ShAd/reboot.sh $delay $check $flag $loop $bmc_reset
   cd - >/dev/null
elif [[ $item == "DC" ]];then
   echo "DC test start"
   if [[ -z "$mode" ]];then
      mode="UTC"
   fi
   if [[ -z "$wait" ]];then
      wait=120                 ##the default wait time of shutdown is 120s
   else
      expr $wait "+" 10 &> /dev/null
      if [ $? -ne 0 ];then
         echo "the input S5 delay time isn't a number,exit" 
         exit
      fi 
   fi
   if [[ $mode == "UTC" ]];then
      cp $Cur_Dir/sh/mode/reboot_sync.sh $ShAd/reboot.sh
   elif [[ $mode == "RTC" ]];then
      cp $Cur_Dir/sh/mode/reboot_BIOS.sh $ShAd/reboot.sh
   else
      echo "the DC mode isn't supported, only utc or rtc, exit now."
      exit
   fi
   ############Info Collect####################
   InfoCheck
   cd $ShAd >/dev/null
   sh $ShAd/reboot.sh $delay $wait $check $flag $loop $bmc_reset
   cd - >/dev/null
elif [[ $item == "AC" ]];then
   echo "AC test start"
    if [[ -z "$mode" ]];then
        mode="PDU"
    elif [[ $mode != "APC" ]]&&[[ $mode != "IPS" ]];then
        if [[ "$mode" =~ "PDU" ]]; then
    		echo "use the ACServer test$mode to control PDU"
	    elif [[ "$mode" == "SINGLE" ]]; then
		    echo "use the single to single pdu to ac test"
	    else
      		echo "The input mode is unsupported, only APC or PDU or IPS or SINGLE,exit now."
      		exit
	    fi
    fi
    if [[ "$mode" =~ "PDU" ]]; then
        if [[ -z "$server_ip" ]]; then
            echo "pls input the AC server ip, for example: -s 10.0.21.1,exit now."
            exit
        else
      	    ping -c5 -i 0.1 $server_ip 1>/dev/null 2>/dev/null
      	    if [ $? -ne 0 ];then
                echo "Cannot connect to server $server_ip, pls check network connection"
         	    exit
      	    fi
        fi
        if [[ -z "$port" ]];then
            echo "pls input the AC port, for example: -p 0,1,exit now."
            exit
        fi
        if [[ -z "$acserverport" ]];then
            echo "pls specify the ac server port, for example: -a 8802 exit now."
            exit
        fi
        cp $Cur_Dir/sh/mode/reboot_AC.sh $ShAd/reboot.sh
        ###Info Collect###
        InfoCheck
        cd $ShAd >/dev/null
        sh $ShAd/reboot.sh $delay $check $flag $loop $mode "$port" $server_ip $acserverport $safe
        cd - >/dev/null
        exit 1
    elif [[ "$mode" == "SINGLE" ]]; then
	    echo "$sysStaticIP system $blackBoxStaticIP box"
        if [[ "$sysStaticIP" != "" ]] && [[ "$blackBoxStaticIP" != "" ]]; then
            if [[ "$sysStaticIP" != "$blackBoxStaticIP" ]];then
                ping -c5 -i 0.1 $blackBoxStaticIP 1>/dev/null 2>/dev/null
                if [ $? -ne 0 ];then
                    echo "Cannot connect to black small box $blackBoxStaticIP, pls check network connection"
                    exit
                fi
                cp $Cur_Dir/sh/mode/reboot_AC.sh $ShAd/reboot.sh
                ###Info Collect###
                InfoCheck
                cd $ShAd
                sh $ShAd/reboot.sh $delay $check $flag $loop $blackBoxStaticIP $safe
                cd -
            else
                echo "you specify the system static ip equal small black box ip,please change one of them"
                exit 1
            fi
        else
            if [[ "$blackBoxStaticIP" != "" ]]; then
                findEth=findAvailableEth
                if [[ "$findEth" == "" ]]; then
                    echo "you have not available ehternet port to use,please add 1G nic"
                    exit 1
                else
                    setStaticEth "$findEth"
                    ifdown "$findEth" 1>/dev/null 2>/dev/null
                    if [ $? -ne 0 ];then
                        echo "shutdown $findEth fail please check network status"
                        recoverEth "$findEth"
                        exit 1
                    fi
                    ifup "$findEth"
                    if [ $? -ne 0 ];then
                        echo "ifup $findEth fail please check network status"
                        recoverEth "$findEth"
                        exit 1
                    fi
                    ping -c5 -i 0.1 $blackBoxStaticIP 1>/dev/null 2>/dev/null
                    if [ $? -ne 0 ];then
                        echo "Cannot connect to black small box $blackBoxStaticIP, pls check network connection"
                        recoverEth "$findEth"
                        exit
                    fi
                    cp $Cur_Dir/sh/mode/reboot_AC.sh $ShAd/reboot.sh
                    ###Info Collect###
                    InfoCheck
                    cd $ShAd
                    sh $ShAd/reboot.sh $delay $check $flag $loop $blackBoxStaticIP $safe
                    cd -
                fi
           else
                echo "you have not specify the black small box ip,please check"
                exit 1
           fi
        fi
    else
        echo "you are not specify the AC test mode,please specify one"
        exit 1
    fi
else
   echo "Unsupport test type,exit."
fi
