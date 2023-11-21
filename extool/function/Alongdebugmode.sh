#! /bin/bash
if [ -z $ipwd ]
then
    ipwd="admin"
fi

if [ -z $iuser ]
then
    iuser="admin"
fi

DebugPhaseString="Debug Phase:"
PeiDebugLevelString="PEI Phase:"
DxeDebugLevelString="DXE Phase:"

PeiPhase=1
DxePhase=2
RTSPhase=4

WarnDebugSupport=2
InfoDebugSupport=0x40
ErrorDebugSupport=8

DebugPhase=0
PeiDebugLevelByte0=0
PeiDebugLevelByte5=0

DxeDebugLevelByte0=0
DxeDebugLevelByte5=0
if [ X$1 != X ]
then
    result=$(echo $1 |grep "pei")
    if [ X$result != X ]
    then
        DebugPhaseString=$DebugPhaseString"<PEI>"
        DebugPhase=$[$DebugPhase+$PeiPhase]
        if [ X$2 != X ]
        then
            result=$(echo $2 |grep "warn")
            if [ X$result != X ]
            then
                PeiDebugLevelString=$PeiDebugLevelString"<Warning>"
                PeiDebugLevelByte0=$[$PeiDebugLevelByte0+$WarnDebugSupport]
            fi

            result=$(echo $2 |grep "info")
            if [ X$result != X ]
            then
                PeiDebugLevelString=$PeiDebugLevelString"<Information>"
                PeiDebugLevelByte0=$[$PeiDebugLevelByte0+$InfoDebugSupport]
            fi

            result=$(echo $2 |grep "err")
            if [ X$result != X ]
            then
                PeiDebugLevelString=$PeiDebugLevelString"<Error>"
                PeiDebugLevelByte5=$[$PeiDebugLevelByte5+$ErrorDebugSupport]
            fi
        fi
    fi

    result=$(echo $1 |grep "dxe")
    if [ X$result != X ]
    then
        DebugPhaseString=$DebugPhaseString"<DXE>"
        DebugPhase=$[$DebugPhase+$DxePhase]
        if [ X$2 != X ]
        then
            result=$(echo $2 |grep "warn")
            if [ X$result != X ]
            then
                DxeDebugLevelString=$DxeDebugLevelString"<Warning>"
                DxeDebugLevelByte0=$[$DxeDebugLevelByte0+$WarnDebugSupport]
            fi

            result=$(echo $2 |grep "info")
            if [ X$result != X ]
            then
                DxeDebugLevelString=$DxeDebugLevelString"<Information>"
                DxeDebugLevelByte0=$[$DxeDebugLevelByte0+$InfoDebugSupport]
            fi

            result=$(echo $2 |grep "err")
            if [ X$result != X ]
            then
                DxeDebugLevelString=$DxeDebugLevelString"<Error>"
                DxeDebugLevelByte5=$[$DxeDebugLevelByte5+$ErrorDebugSupport]
            fi
        fi
    fi

    result=$(echo $1 |grep "rts")
    if [ X$result != X ]
    then
        DebugPhaseString=$DebugPhaseString"<RTS>"
        DebugPhase=$[$DebugPhase+$RTSPhase]
    fi

    result=$(echo $1 |grep "-h")
    if [ X$result != X ]
    then
        echo "Alongdebugmode:Set Debug Mode Via BMC Interface."
        echo "Example:
                ./Alongdebugmode.sh pei-dxe-rts warn-error-info  ==> Set Debug Phase & Level
                ./Alongdebugmode.sh ==> Disable Debug"
    fi
fi
echo ------------------------------
echo ipmitool -I lanp -H $IP -U $iuser -P $ipwd raw 0x3a 0xc0 0x00 14  0x01     $DebugPhase 0x55 0x55 0x55   $PeiDebugLevelByte0 0x55 0x55 0x55 $PeiDebugLevelByte5   $DxeDebugLevelByte0 0x55 0x55 0x55 $DxeDebugLevelByte5   0x55
echo ------------------------------
time2=$(date "+%Y-%m-%d-%H:%M:%S")
echo $time2 
echo ------------------------------
ipmitool -I lanp -H $IP -U $iuser -P $ipwd raw 0x3a 0xc0 0x00 14  0x00     0x00 0x00 0x00 0x00   0x00 0x00 0x00 0x00 0x00   0x00 0x00 0x00 0x00 0x00   0x00
if [ X$DebugPhase != X0 ]
then
    echo $DebugPhaseString
    echo $PeiDebugLevelString
    echo $DxeDebugLevelString

    ipmitool -I lanp -H $IP -U $iuser -P $ipwd raw 0x3a 0xc0 0x00 14  0x01     $DebugPhase 0x55 0x55 0x55   $PeiDebugLevelByte0 0x55 0x55 0x55 $PeiDebugLevelByte5   $DxeDebugLevelByte0 0x55 0x55 0x55 $DxeDebugLevelByte5   0x55
fi