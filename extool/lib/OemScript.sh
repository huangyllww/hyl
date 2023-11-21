#! /bin/bash
if [ -z $ipwd ]
then
    ipwd="admin"
fi

if [ -z $iuser ]
then
    iuser="admin"
fi

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
    ipmitool -I lanplus -U $iuser -P $ipwd -H $bmcip raw 0x32 0x73 $1
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
        ipmitool -I lanplus -U $iuser -P $ipwd -H $bmcip sol $1
        return
    fi
    
    if [ $# -eq 2 ]
    then
        dumpstring=$dumpstring"Set SOL $1 !!\nLog Path: $2\n"
        AccordedTime=$(date "+%Y-%m-%d-%H:%M:%S")
        dumpstring=$dumpstring" ("$AccordedTime")" 
        echo -e $dumpstring
        echo "----------------"
        ipmitool -I lanplus -U $iuser -P $ipwd -H $bmcip sol $1 |tee -a $2.log
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
    ipmitool -I lanp -H $bmcip -U $iuser -P $ipwd raw 0x3a 0xc0 0x00 14  0x00     0x00 0x00 0x00 0x00   0x00 0x00 0x00 0x00 0x00   0x00 0x00 0x00 0x00 0x00   0x00
    
    #if [ $# -eq 2 ]
    if [ $# -ne 0 ]
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
        if [ $# -eq 2 ]
        then
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
    fi
    ##Set Debug Mode
    AccordedTime=$(date "+%Y-%m-%d-%H:%M:%S")
    if [ $# -ne 0 ]
    then
        if [ $# -eq 1 ]
        then
            DebugLevelByte0=0x55
            DebugLevelByte5=0x55
        fi
        dumpstring=$dumpstring" ("$AccordedTime")\n"$debugphasestring"|"$DebugPhase"    "$debuglevelstring"|"$DebugLevelByte0" "$DebugLevelByte5
    else
        dumpstring=$dumpstring" ("$AccordedTime")"
    fi
    echo -e $dumpstring
    echo "----------------"
    if [ $# -ne 0 ]
    then
        ipmitool -I lanp -H $bmcip -U $iuser -P $ipwd raw 0x3a 0xc0 0x00 0x0e  0x01  $DebugPhase 0x55 0x55 0x55 $DebugLevelByte0 0x55 0x55 0x55 $DebugLevelByte5 $DebugLevelByte0 0x55 0x55 0x55 $DebugLevelByte5 0x55
    fi
}


function sdebugmode()
{
    
    dumpstring='Host IP:'$bmcip"\n"
    debugphasestring=""
    debuglevelstring=""

    Byte5_PEI=1
    Byte5_DXE=2
    Byte5_RTS=4

    Byte6_NO=0x0
    Byte7_NO=0x0
    Byte8_NO=0x0

    Byte9_WARN=2
    Byte9_INFO=0x40

    Byte10_NO=0x0
    Byte11_NO=0x0
    Byte12_ERR=0x80
    Byte13_NO=0x0
    Byte14_NO=0x0
    Byte15_NO=0x0
    Byte16_NO=0x0
    Byte17_NO=0x0
    Byte18_NO=0x0
    Byte19_NO=0x0
    Byte20_NO=0x0

    Byte5_DebugPhase=0
    Byte9_DebugLevel=0
    Byte12_DebugLevel=0

    if [ $# -eq 0 ]
    then
        dumpstring=$dumpstring"Disable Debug Mode!!"
    fi
    #Disable First
    ipmitool -I lanp -H $bmcip -U $iuser -P $ipwd raw 0x3a 0xc0 0x00 14  0x00     0x00 0x00 0x00 0x00   0x00 0x00 0x00 0x00 0x00   0x00 0x00 0x00 0x00 0x00   0x00
    
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
                        Byte5_DebugPhase=$[$Byte5_DebugPhase+$Byte5_PEI]
                        ;;
                    "dxe")
                        Byte5_DebugPhase=$[$Byte5_DebugPhase+$Byte5_DXE]
                        ;;
                    "rts")
                        Byte5_DebugPhase=$[$Byte5_DebugPhase+$Byte5_RTS]
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
                        Byte12_DebugLevel=$[$Byte12_DebugLevel+$Byte12_ERR]
                        ;;
                    "info")
                        Byte9_DebugLevel=$[$Byte9_DebugLevel+$Byte9_INFO]
                        ;;
                    "warning")
                        Byte9_DebugLevel=$[$Byte9_DebugLevel+$Byte9_WARN]
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
        ipmitool -I lanp -H $bmcip -U $iuser -P $ipwd raw 0x3a 0xc0 0x00 0x0e  0x01  $Byte5_DebugPhase $Byte6_NO $Byte7_NO $Byte8_NO $Byte9_DebugLevel $Byte10_NO $Byte11_NO $Byte12_DebugLevel $Byte13_NO $Byte14_NO $Byte15_NO $Byte16_NO $Byte17_NO $Byte18_NO $Byte19_NO
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
    ipmitool -I lanp -H $bmcip -U $iuser -P $ipwd chassis $*
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
    ipmitool -I lanp -H $bmcip -U $iuser -P $ipwd  $*
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
        spawn ssh simsadmin@10.8.102.95 -p 51730;
        expect {
            "yes/no" { send \"yes\r\"; exp_continue }
            "password" { send \"sims@Admin#xyz\r\" }  
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
        spawn ssh root@$osip;
        expect {
            "yes/no" { send \"yes\r\"; exp_continue }
            "password" { send \"\Suma@123\r\"}
        }
        interact
    "
}

function dump()
{
	echo "BMC IP:"$bmcip
    echo "os  IP:"$osip
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
    ./sistool setconf bios -H $bmcip -U $iuser -P $ipwd -A $1 -V $2
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
    ./sistool getconf bios -H $bmcip -U $iuser -P $ipwd -O SearchBIOSConfResult > /dev/null
    cat SearchBIOSConfResult|grep -i $1 -B1
    rm SearchBIOSConfResult
}

function BIOSFWUpdate()
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
    
    if [  $# -gt 1 ];then
    ./sistool update bios -N -H $bmcip -U $iuser -P $ipwd -I 999_FWupdate/$1.*
    else
    ./sistool update bios -N -H $bmcip -U $iuser -P $ipwd -I 999_FWupdate/$1.ROM
    fi
    
    cat sistlogs/*.* |grep -i Result
    rm -rf sistlogs
}

function SerialServer()
{
    dumpstring='Host IP:'$bmcip"\n"
    ##Set Debug Mode
    AccordedTime=$(date "+%Y-%m-%d-%H:%M:%S")
    dumpstring=$dumpstring$*
    dumpstring=$dumpstring" ("$AccordedTime")"
    echo -e $dumpstring
    echo "----------------"

    SerialServerIP=$(echo $1)
    SerialServerPort=$(echo $2)
    if [  $# -gt 1 ];then
        nc $SerialServerIP $SerialServerPort
    else
        nc $SerialServerIP $SerialServerPort |tee $3
    fi
}

function MonitorOOBTransfer()
{
    #for ((i=1;;i++))
    #do
    SelTimeString=$( ipmi sel elist|grep -i Timestamp)
    if [ X$SelTimeString  != X ]
    then
        echo "Start MonitorOOBTransfer Entry!!"
    else
        echo "Exit Current Loop!!"
        return
    fi
    echo "Continue To Dump OOB Data - BiosAttributeRegistryCXYH0..."
    expect -c "
        set timeout 30;
        spawn scp sysadmin@$bmcip:/conf/redfish/bios/BiosAttributeRegistryCXYH0.0.15.0.json BiosAttributeRegistryCXYH0.0.15.0_$1.json;
        expect {
            "yes/no" { send \"yes\r\"; exp_continue }
            "password" { send \"superuser\r\" }  
        }
        interact
    "
    #OOBCompare=$( diff BiosAttributeRegistryCXYH0.0.15.0.json Org_BiosAttributeRegistryCXYH0.0.15.0.json)
    #if [ X$OOBCompare  != X ]
    #then
    #    echo "File:BiosAttributeRegistryDXYH0 Mismatch,Exit!!"
    #    return
    #fi

    echo "Continue To Dump OOB Data - bios_current_settings..."
    expect -c "
        set timeout 30;
        spawn scp sysadmin@$bmcip:/conf/redfish/bios/bios_current_settings.json bios_current_settings_$1.json;
        expect {
            "yes/no" { send \"yes\r\"; exp_continue }
            "password" { send \"superuser\r\" }  
        }
        interact
    "
    #OOBCompare=$( diff bios_current_settings.json Org_bios_current_settings.json)
    #if [ X$OOBCompare  != X ]
    #then
    #    echo "File:Org_bios_current_settings Mismatch,Exit!!"
    #    return
    #fi

    #rm BiosAttributeRegistryCXYH0.0.15.0.json
    #rm bios_current_settings.json
    ipmi sel clear

    echo "Set Save Config"
    ipmi raw 0x3a 0xa4 1
    #sleep 60
    #done
}

function OOBTest()
{
    for ((i=1;i>0;i++)) 
    do
        MonitorOOBTransfer $i
        sleep 30
    done
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
    "OOBTest")
        MonitorOOBTransfer
        ;;
    "solloop")
        for i in {1..100}
        do 
            sol deactivate
            sol activate $2
        done
        ;;
esac
