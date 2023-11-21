################################################################
###
### Enable or disable the debug mode
###
### Parameter: $1 = debug phase; $2 = debug level
### 
################################################################


#included files

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


################################################################
###
### 补全指令
### 
################################################################


_debugmode()
{  
	local cur prev 
	COMPREPLY=()

	cur="${COMP_WORDS[COMP_CWORD]}"
	prev="${COMP_WORDS[COMP_CWORD-1]}"

	if [ "$prev" = 'debugmode' ]
	then
		COMPREPLY=( $(compgen -W "peidxerts peirtsdxe peidxe peirts dxertspei dxepeirts rtsdxepei rtspeidxe" $cur ))
	else
		COMPREPLY=( $(compgen -W "error warningerror infowariningerror" $cur ))
	fi

	return 0
}

complete -F _debugmode debugmode