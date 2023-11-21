#! /bin/bash


#pravity function list Start
0biosfwupdate()
{ 
	biosfwupdate $1
	chassis power on
}
# 0debugmode() { debugmode 
# }
# 0cpuunlock() { cpuunlock 
# }
# 0autolog() { autolog 
# }


0help()
{
    echo "CMD List:"
    cat  OemScript.sh|grep -i function | awk -F ' ' '$1=="function"{print "  func: "$2}' 
}

_InvalidParameter()
{
    echo "------------------------"
    echo "Invalid parameter!!"
    echo "------------------------" 
    return 0
}

dump()
{
    if [ $# -eq 0 ]
    then
        echo "------------------------" 
		echo "BMC ip : "$bmcip
        echo "BMC usr: "$bmcusr
        echo "BMC pw : "$bmcpw
        echo "OS  ip : "$osip
        echo "OS  usr: "$osusr
        echo "OS  pw : "$ospw
        echo "------------------------"  
    fi 
}


_Realign()
{
    #整理配置文件内容格式
    #找#:bmcip= ，没有则从第一行开始
    #首先筛选出全部bmcip所在行数，并赋值给变量
    RealignBmcipArray="$(sed -n "/#:bmcip/=" $CONF_DIR/EnvConfExt)"
    
    #初始化组数=1
    RealignGroupNum=1

    while [ -n "$RealignBmcipArray" ]
    do
        #提取配置文件中的第一个组标识符 #N#:
        RealignBmcipFirstLineNumber=$(echo $RealignBmcipArray | awk -F' ' '{print $1}')
        RealignBmcipArray=${RealignBmcipArray#*"$RealignBmcipFirstLineNumber"}
        
        #读取这一行内容
        RealignFirstLineContext=$(cat $CONF_DIR/EnvConfExt | sed -n "$RealignBmcipFirstLineNumber,$RealignBmcipFirstLineNumber p")
        RealignGroupFlag=$(echo $RealignFirstLineContext | awk -F"bmcip=" '{print $1}')

        #替换本组的全部组标识符
        RealignAfterGroupStr="#$RealignGroupNum#:"

		i=0
		while [ $i -lt 6 ]
		do
			sed -in "s/$RealignGroupFlag/$RealignAfterGroupStr/g" $CONF_DIR/EnvConfExt
			i=$[1+$i]
		done

        #组数+1
        RealignGroupNum=$[1+$RealignGroupNum]

    done
    #重排结束
}

_AddBmcipdefault()
{
    bmcusr=admin
    bmcpw=admin
    osip=
    osusr=root
    ospw=Suma@123
    dump
    echo "###:bmcip=$bmcip" >> $CONF_DIR/EnvConfExt
    echo "###:bmcusr=$bmcusr" >> $CONF_DIR/EnvConfExt
    echo "###:bmcpw=$bmcpw" >> $CONF_DIR/EnvConfExt
    echo "###:osip=$osip" >> $CONF_DIR/EnvConfExt
    echo "###:osusr=$osusr" >> $CONF_DIR/EnvConfExt
    echo "###:ospw=$ospw" >> $CONF_DIR/EnvConfExt
    echo '' >> $CONF_DIR/EnvConfExt

    #整理配置文件内容格式
    _Realign
}


#需要传入1个参数
_CheckIpValid()  
{
    local DefaultIpArray=(10 8 140 255)
    local NewIpArray=(0)
    local NewArrayNum=0
    local ValidFlag=0
    local i=0 j=4

    #对IFS变量 进行替换处理
    OLD_IFS="$IFS"  #保存当前shell默认的分割符，一会要恢复回去
    IFS='.'                  #将shell的分割符号改为，“”
    NewIpArray=($1)     #分割符是“.”赋值给array 就成了数组赋值
    IFS="$OLD_IFS"  #恢复shell默认分割符配置

    NewArrayNum=${#NewIpArray[@]}

    i=$NewArrayNum
    #校验输入ip是否合法
    while [ $i -ge 1 -a $i -le $NewArrayNum ]
    do
        if [ ${NewIpArray[i-1]} -gt 0 -a ${NewIpArray[i-1]} -lt 255 ]
        then
            ValidFlag=$[$ValidFlag+1]
        fi
        i=$[$i-1]
    done

    if [ $ValidFlag -eq $NewArrayNum ]
    then
        i=$NewArrayNum
        j=4
        while [ $i -ge 1 -a $i -le $NewArrayNum ]
        do
            DefaultIpArray[j-1]=${NewIpArray[i-1]}
            j=$[$j-1]
            i=$[$i-1]
        done
        NewIp=${DefaultIpArray[0]}.${DefaultIpArray[1]}.${DefaultIpArray[2]}.${DefaultIpArray[3]}
        return 1
    else
        _InvalidParameter
    fi
}




function cpuunlock()
{
    echo "----------------"	
	echo -e "BMC ip :$bmcip is unlock"
    echo "----------------"	
	ipmitool -I lanplus -U $bmcusr -P $bmcpw -H $bmcip raw 0x32 0x43 0 1 
}


function varset()
{
    if [ $# -eq 2 ]
    then
        local ValidFlag=0
        local var1=$1
        local var2=$2
        case $1 in
            "bmcip")
                _CheckIpValid $var2
                var2=$NewIp
                ValidFlag=1
                ;;
            "bmcusr")
                ValidFlag=1
                ;;
            "bmcpw")
                ValidFlag=1
                ;;
            "osip")
                _CheckIpValid $var2
                var2=$NewIp
                ValidFlag=1
                ;;
            "osusr")
                ValidFlag=1
                ;;
            "ospw")
                ValidFlag=1
                ;;  
            *) 
                ;;     
        esac

        #根据校验结果，选择是否执行操作
        if [ $ValidFlag -eq 0 ]  #无效
        then
            _InvalidParameter 
        else
            export $var1=$var2

            #组装Tag
            ExportGroupTag="#$globalGroupNum#:$var1"
            
            #找到line
            ExportFindLineNumExt=$(echo $(sed -n "/$ExportGroupTag/=" $CONF_DIR/EnvConfExt) | awk -F' ' '{print $1}')
            ExportReplaceLineContextExt=$(cat $CONF_DIR/EnvConfExt | sed -n "$ExportFindLineNumExt,$ExportFindLineNumExt p")

            #更新配置文件
            ExportContext="#$globalGroupNum#:$var1=$var2"
            sed -in "$ExportFindLineNumExt c "$ExportContext"" $CONF_DIR/EnvConfExt

            #回显修改值
            cat $CONF_DIR/EnvConfExt | grep $ExportGroupTag

			#重排配置文件
			_Realign
        fi

    else
        echo "------------------------"
        echo "Invalid parameter!!"
        echo "Input: varset <tab><tab> [new value]"
        echo "------------------------" 
    fi 
}


function iplist()
{
     
    if [ $# -eq 0 ]
    then
        echo "------------------------"
		cat $CONF_DIR/EnvConfExt | grep -i bmcip
		echo "------------------------"
    else
        #新增功能： add, delete, os
        case $1 in
            "addip")
                #AddBmcipValidFlag=1
                if [ $# -eq 1 ]
                then
                    echo "------------------------"
                    echo "Input: iplist add <bmcip> "
                    echo "------------------------"
                elif [ $# -eq 2 ]
                then
                    _CheckIpValid $2
                    bmcip=$NewIp
                    echo $bmcip
                    _AddBmcipdefault

                else #参数数量不是1或2
                    _InvalidParameter
                fi
                ;;
            "delete")
                #删除某一组数据

                if [ $# -eq 2 ]
                then
                    #找到这一组的bmcip，并询问是否删除
                    if [ $2 -ge 0 -a $2 -le 255 ]
                    then
                        DeleteGroupIdStr="#$2#:"
                        DeleteGroupBmcipLine=$(echo $(sed -n "/$DeleteGroupIdStr/=" $CONF_DIR/EnvConfExt) | awk -F' ' '{print $1}')
                        DeleteGroupBmcipLineContext=$(cat $CONF_DIR/EnvConfExt | sed -n "$DeleteGroupBmcipLine,$DeleteGroupBmcipLine p")  
                        DeleteBmcip=${DeleteGroupBmcipLineContext#*'bmcip='} 
            
                        if [ -n "$DeleteGroupBmcipLine" ]
                        then
                            read -r -p "#$2# Host:$DeleteBmcip while be deleted, are you sure? [Y/n] " input
                            case $input in
                                [yY][eE][sS]|[yY])
                                    #执行删除动作
                                    DeleteGroupBmcipLine=$[6+DeleteGroupBmcipLine]
                                    
                                    sed -i "$DeleteGroupBmcipLine d" $CONF_DIR/EnvConfExt
                                    sed -i "/$DeleteGroupIdStr/d" $CONF_DIR/EnvConfExt
                                    echo "#$2# Host:$DeleteBmcip is deleted!"

                                    #整理配置文件格式
                                    _Realign
                                    ;;

                                [nN][oO]|[nN])
                                    #echo "No"
                                    ;;

                                *)
                                    echo "Invalid input..."
                                    exit 1
                                    ;;
                            esac
                        else
                            echo "------------------------" 
                            echo "no such group !!"
                            echo "------------------------" 
                        fi
                    fi
                elif [ $# -eq 1 ]
                then
                    echo "------------------------" 
                    cat $CONF_DIR/EnvConfExt | grep -i bmcip
                    echo "------------------------" 
                    echo "Input: iplist del <Group number>"
                    echo "------------------------"  
                else
                    _InvalidParameter
                fi
                ;;

            "oslist")
                echo "------------------------"
                cat $CONF_DIR/EnvConfExt | grep -i osip
                echo "------------------------"
                ;;

            "dump")
                echo "------------------------" 
                echo " Current Envariment:" 
                dump 
                ;;

            "scan")
                #AddBmcipValidFlag=1
                if [ $# -eq 1 ]
                then
                    echo "------------------------"
                    echo "Input: iplist scan bmc/os "
                    echo "------------------------"
                elif [ $# -eq 2 ]
                then
                    case $2 in
                        "bmc")
                        #扫描全部IP
                        r='full'
                        while [ -n "$r" ]
                        do
                            i=1
                            r=$(cat $CONF_DIR/EnvConfExt | awk "/bmcip=/{i++}i==$i{print; exit}" | awk -F'bmcip=' '{print $2}')
                            i=$[$i+1]
                        done
                        
                        ;;

                        "os")
                        cat $CONF_DIR/EnvConfExt | grep -i -m 1 osip= | awk -F'osip=' '{print $2}'
                        ;;

                        *)
                            _InvalidParameter
                        ;;
                    esac

                else #参数数量不是1或2
                    _InvalidParameter
                fi
                ;;

            *)
                echo "------------------------" 
                echo "Invalie parameter!!"
                echo "option: add, del, os"
                echo "------------------------" 
                ;;
        esac
    fi 
}


function sw()
{
    # 函数功能：切换保存的环境
    # 当前限制：判断标准为ip的最后一段，若配置文件中存在多组ip，以第一个为准
    # 因此需要注意避免ip冲突

    if [ $# -eq 1 -a $1 -ge 0 -a $1 -le 255 ]
    then
        NotFountFlag=1
        InputStr=$1

        #首先筛选出全部bmcip所在行数，并赋值给变量
        LineOfBmcipArray=$(sed -n "/:bmcip/=" $CONF_DIR/EnvConfExt)

        #bmcip行不为空时，进行详细搜索：遍历全部bmcip所在行，寻找输入的ip
        while [ -n "$LineOfBmcipArray" ]
        do
            CurrentLineOfBmcip=$(echo $LineOfBmcipArray | awk -F' ' '{print $1}')
            LineOfBmcipArray=${LineOfBmcipArray#*"$CurrentLineOfBmcip"} 
            CurrentLineContext=$(cat $CONF_DIR/EnvConfExt | sed -n "$CurrentLineOfBmcip,$CurrentLineOfBmcip p")      
            CurrentLineIdStr=$(echo $CurrentLineContext | awk -F'.' '{print $4}')

            #获取当前组号
            CurrentGroupNum=$(echo $CurrentLineContext | awk -F'#:bmcip' '{print $1}')
            CurrentGroupNum=$(echo $CurrentGroupNum | awk -F'#' '{print $2}')

            #判断字符串是否为空，null时跳过，
            if [ -n "$CurrentLineIdStr" -a "$CurrentLineIdStr" = "$InputStr" ] 
            then
                #找到行结果,并初始化flag
                NotFountFlag=0 #清除该flag
                VarNumber=1

                #更新环境和配置文件
                while [ $VarNumber -le 6 ]
                do
                    #清除前四个字符
                    CurrentLineContext=${CurrentLineContext#*'#:'} 
                    #export
                    VarName=$(echo $CurrentLineContext | awk -F'=' '{print $1}')
                    VarValue=$(echo $CurrentLineContext | awk -F'=' '{print $2}')
                    export $VarName=$VarValue

                    # VarNumber++; CurrentLineOfBmcip++;
                    VarNumber=$[1+$VarNumber]
                    CurrentLineOfBmcip=$[1+$CurrentLineOfBmcip]
                    CurrentLineContext=$(cat $CONF_DIR/EnvConfExt | sed -n "$CurrentLineOfBmcip,$CurrentLineOfBmcip p")
                done 
                
                #清除数组,更新全局变量globalGroupNum，结束搜索
                LineOfBmcipArray=''
                globalGroupNum=$CurrentGroupNum
                globalGroupNumContext="globalGroupNum=$globalGroupNum"
                sed -in "1c "$globalGroupNumContext"" $CONF_DIR/EnvConfExt

                #dump ip信息
                echo "------------------------" 
                echo "BMC ip : "$bmcip
                echo "OS  ip : "$osip
                echo "------------------------" 
            fi

            #该行不是所找行，继续搜索下一行
        done

        if [ $NotFountFlag -eq 1 ]
        then
            echo "------------------------" 
            echo "no such ip!!"
            echo "------------------------" 
        fi
    fi
}


function autolog()
{
    dumpstring='Host IP:'$bmcip"\n"
    AccordedTime=$(date "+%Y-%m-%d-%H:%M:%S")
    dumpstring=$dumpstring" ("$AccordedTime")" 

    if [ $# -eq 0 ]
    then
        LogName=$(date "+%Y%m%d_%H%M%S")
    elif [ $# -eq 1 ]
    then
        

		case $1 in
			"showonly")
				#判断是否输入参数，例如输入有效文件名
				echo -e $dumpstring
				echo "----------------"
				while [[ 1 ]] ; do ipmitool -I lanplus -U $bmcusr -P $bmcpw -H $bmcip sol deactivate; ipmitool -I lanplus -U $bmcusr -P $bmcpw -H $bmcip sol activate; done
				return 0
				;;
			*)
                LogName=$1
                ;;
		esac
    else
        echo "------------------------"
        echo "Invalid file name!!"
        echo "------------------------" 
        return 0
    fi

    #判断是否输入参数，例如输入有效文件名
    echo -e $dumpstring
    dumpstring='log Path:/home/cancon/autolog/'$LogName'.log'"\n"
    echo -e $dumpstring
    echo "----------------"
	while [[ 1 ]] ; do ipmitool -I lanplus -U $bmcusr -P $bmcpw -H $bmcip sol deactivate; ipmitool -I lanplus -U $bmcusr -P $bmcpw -H $bmcip sol activate; done | tee /home/cancon/autolog/$LogName.log
    return 0
}
#pravity function list End


#function list
function 80port()
{
    #
    # Info
    #
    dumpstring='Host IP:'$bmcip"\n"
    if [ $# -eq 0 ]
    then
        echo "No Valid Paramter Input!!!"
        return
    fi

    case $1 in
        "0")
            dumpstring=$dumpstring"Dump Current Port80 Data"
            ;;
        "1")
            dumpstring=$dumpstring"Dump Privious Port80 Data"
            ;;
    esac

    AccordedTime=$(date "+%Y-%m-%d-%H:%M:%S")
    dumpstring=$dumpstring" ("$AccordedTime")" 
    echo -e $dumpstring 
    echo "----------------"
    ipmitool -I lanplus -U $bmcusr -P $bmcpw -H $bmcip raw 0x32 0x73 $1
}



function sol()
{
    
    dumpstring='Host IP:'$bmcip"\n"

    if [ $# -eq 0 ]
    then
        echo "No Valid Paramter Input!!!"
        return
    fi

    if [ $# -eq 1 ]
    then
        dumpstring=$dumpstring"Set SOL $1 !!"
        AccordedTime=$(date "+%Y-%m-%d-%H:%M:%S")
        dumpstring=$dumpstring" ("$AccordedTime")" 
        echo -e $dumpstring 
        echo "----------------"
        ipmitool -I lanplus -U $bmcusr -P $bmcpw -H $bmcip sol $1
        return
    fi
    
    if [ $# -eq 2 ]
    then
        dumpstring=$dumpstring"Set SOL $1 !!\nLog Path: $2\n"
        AccordedTime=$(date "+%Y-%m-%d-%H:%M:%S")
        dumpstring=$dumpstring" ("$AccordedTime")" 
        echo -e $dumpstring
        echo "----------------"
        ipmitool -I lanplus -U $bmcusr -P $bmcpw -H $bmcip sol $1 |tee -a /home/cancon/log$2.log
    fi
}


function debugmode()
{
    
    dumpstring='Host IP:'$bmcip"\n"
    debugphasestring=""
    debuglevelstring=""

    PeiPhase=1
    DxePhase=2
    RTSPhase=4

    WarnDebugSupport=2
    InfoDebugSupport=0x40
    ErrorDebugSupport=8

    DebugPhase=0
    DebugLevelByte0=0
    DebugLevelByte5=0

    if [ $# -eq 0 ]
    then
        dumpstring=$dumpstring"Disable Debug Mode!!"
    fi
    #Disable First
    ipmitool -I lanp -H $bmcip -U $bmcusr -P $bmcpw raw 0x3a 0xc0 0x00 14  0x00     0x00 0x00 0x00 0x00   0x00 0x00 0x00 0x00 0x00   0x00 0x00 0x00 0x00 0x00   0x00
    
    if [ $# -eq 2 ]
    then
        dumpstring=$dumpstring"Set Debug Mode!!"
        for debugphase in {"pei","dxe","rts"}
        do
            debugphaseresult=$(echo $1 |grep $debugphase)
            if [ X$debugphaseresult != X ]
            then
                debugphasestring=$debugphasestring"<$debugphase>"

                case $debugphase in
                    "pei")
                        DebugPhase=$[$DebugPhase+$PeiPhase]
                        ;;
                    "dxe")
                        DebugPhase=$[$DebugPhase+$DxePhase]
                        ;;
                    "rts")
                        DebugPhase=$[$DebugPhase+$RTSPhase]
                        ;;
                esac
            fi
        done

        for debuglevel in {"error","info","warning"}
        do
            debuglevelresult=$(echo $2 |grep $debuglevel)
            if [ X$debuglevelresult != X ]
            then
                debuglevelstring=$debuglevelstring"<$debuglevel>"

                case $debuglevel in
                    "error")
                        DebugLevelByte5=$[$DebugLevelByte5+$ErrorDebugSupport]
                        ;;
                    "info")
                        DebugLevelByte0=$[$DebugLevelByte0+$InfoDebugSupport]
                        ;;
                    "warning")
                        DebugLevelByte0=$[$DebugLevelByte0+$WarnDebugSupport]
                        ;;
                esac
            fi
        done
    fi
    ##Set Debug Mode
    AccordedTime=$(date "+%Y-%m-%d-%H:%M:%S")
    if [ $# -eq 2 ]
    then
        dumpstring=$dumpstring" ("$AccordedTime")\n"$debugphasestring"|"$DebugPhase"    "$debuglevelstring"|"$DebugLevelByte0" "$DebugLevelByte5
    else
        dumpstring=$dumpstring" ("$AccordedTime")"
    fi
    echo -e $dumpstring
    echo "----------------"
    if [ $# -ne 0 ]
    then
        ipmitool -I lanplus -H $bmcip -U $bmcusr -P $bmcpw raw 0x3a 0xc0 0x00 14  0x01 $DebugPhase 0x55 0x55 0x55 $DebugLevelByte0 0x55 0x55 0x55 $DebugLevelByte5 $DebugLevelByte0 0x55 0x55 0x55 $DebugLevelByte5 0x55
        printf "ipmitool -I lanplus -H $bmcip -U %s -P %s raw 0x3a 0xc0 0x00 0xe\n \33[22;;33m0x01\033[0m \33[22;;32m0x%02x\033[0m 0x55 0x55 0x55 \33[22;;36m0x%02x\033[0m 0x55 0x55 0x55 \33[22;;36m0x%02x\033[0m \33[22;;31m0x%02x\033[0m 0x55 0x55 0x55 \33[22;;31m0x%02x\033[0m 0x55\n" $bmcusr $bmcpw $DebugPhase $DebugLevelByte0 $DebugLevelByte5 $DebugLevelByte0 $DebugLevelByte5
    fi
}


function chassis()
{
    
    dumpstring='Host IP:'$bmcip"\n"
    ##Set Debug Mode
    AccordedTime=$(date "+%Y-%m-%d-%H:%M:%S")
    dumpstring=$dumpstring$*
    dumpstring=$dumpstring" ("$AccordedTime")"
    echo -e $dumpstring
    echo "----------------"
    ipmitool -I lanp -H $bmcip -U $bmcusr -P $bmcpw chassis $*
}


function ipmi()
{
    
    dumpstring='Host IP:'$bmcip"\n"
    ##Set Debug Mode
    AccordedTime=$(date "+%Y-%m-%d-%H:%M:%S")
    dumpstring=$dumpstring$*
    dumpstring=$dumpstring" ("$AccordedTime")"
    echo -e $dumpstring
    echo "----------------"
    ipmitool -I lanp -H $bmcip -U $bmcusr -P $bmcpw  $*
}

function bmclogin()
{
    
    expect -c "
        set timeout 30;
        spawn ssh sysadmin@$bmcip;
        expect {
            "yes/no" { send \"yes\r\"; exp_continue }
            "password" { send \"superuser\r\" }  
        }
        interact
    "
}


function jumperKS()
{
    
    expect -c "
        set timeout 30;
        spawn ssh root@10.8.102.95 -p 5173;
        expect {
            "yes/no" { send \"yes\r\"; exp_continue }
            "password" { send \"sims@Admin#Root\r\" }  
        }
        interact
    "
}

function jumperTJ()
{
    
    expect -c "
        set timeout 30;
        spawn ssh root@10.2.68.128 -p 5173;
        expect {
            "yes/no" { send \"yes\r\"; exp_continue }
            "password" { send \"sims@Admin#Root\r\" }  
        }
        interact
    "
}

function oslogin()
{
    
    expect -c "
        set timeout 30;
        spawn ssh $osusr@$osip;
        expect {
            "yes/no" { send \"yes\r\"; exp_continue }
            "password" { send $ospw\n }
        }
        interact
    "
}


function SetBIOSConf()
{
    
    dumpstring='Host IP:'$bmcip"\n"
    ##Set Debug Mode
    AccordedTime=$(date "+%Y-%m-%d-%H:%M:%S")
    dumpstring=$dumpstring$*
    dumpstring=$dumpstring" ("$AccordedTime")"
    echo -e $dumpstring
    echo "----------------"
    ./sistool setconf bios -H $bmcip -U $bmcusr -P $bmcpw -A $1 -V $2
}

function SearchBIOSConf()
{
    
    dumpstring='Host IP:'$bmcip"\n"
    ##Set Debug Mode
    AccordedTime=$(date "+%Y-%m-%d-%H:%M:%S")
    dumpstring=$dumpstring$*
    dumpstring=$dumpstring" ("$AccordedTime")"
    echo -e $dumpstring
    echo "----------------"
    ./sistool getconf bios -H $bmcip -U $bmcusr -P $bmcpw -O SearchBIOSConfResult > /dev/null
    cat SearchBIOSConfResult|grep -i $1 -B1
    rm SearchBIOSConfResult
}

function biosfwupdate()
{
    
    dumpstring='Host IP:'$bmcip"\n"
    ##Set Debug Mode
    AccordedTime=$(date "+%Y-%m-%d-%H:%M:%S")
    dumpstring=$dumpstring$*
    dumpstring=$dumpstring" ("$AccordedTime")"
    echo -e $dumpstring
    echo "----------------"
    chassis power off 
    rm -rf sistlogs
	./sistool update bios -N -H $bmcip -U $bmcusr -P $bmcpw -I 000_FWupdate/$1*.ROM

    cat sistlogs/*.* |grep -i Result
    rm -rf sistlogs
}


case $1 in
    "-h")
        echo "CMD List:"
        cat  OemScript.sh|grep -i function | awk -F ' ' '{print "   "$2}'
        ;;
    "80")
        80port $2
        ;;
    "bootdev")
        chassis bootdev $2
        ;;
    "ipmi")
        $*
        ;;
    "sol")
        $*
        ;;
    "solloop")
        for i in {1..100}
        do 
            sol deactivate
            sol activate $2
        done
        ;;
esac
