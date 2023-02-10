#!/bin/bash

#################################################
hex()
{
	echo $(printf "%x" $1)
}
#################################################

#################################################
function cleanup()
{
  ROOTDEV=$1

  echo Cleanup done IOHC device is $ROOTDEV. Exiting.

  exit
}

# Ensure that we are running with root privileges
if [ "$EUID" -ne 0 ]
  then echo "Please run as root or using sudo"
  exit
fi

# Version number 
echo "Version 1.3"

# Print out help - TBD do this only when explicitly requested or called incorrectly
echo "Test script to perform LCRC or ACS error injection on PCIe links."
echo "This script injects errors into the selected PCIe die / port."
echo "Requires root privelege to run and use of setpci package."
echo "Requires LCRC or ACS to be enabled in the selected PCIe die/port."
echo "Requires a PCIe device to be installed in the slot that corresponds to the selected die/port, and traffic to be sent to and from that device."
echo "Command line arguments:"
echo "t s d l p"
echo "t = type (lcrc-rx, lcrc-tx, ecrc-rx, ecrc-tx, acs-nonfatal, or acs-fatal)"
echo "s = socket (0 or 1)"
echo "d = die (0, 1, 2, or 3)"
echo "l = pcie leaf (0 or 1)"
echo "p = pcie port (0 through 7)"

#################################################
echo input arg is .........
TYPE=$1
SOCKET=$2
DIE=$3
LEAF=$4
PORT=$5

#################################################

case $SOCKET in
	0)
	  echo Socket $SOCKET
		;;

	1)
	  echo Socket $SOCKET
		;;
	*)
	  echo Invalid Socket: $SOCKET
	  exit
		;;
esac

#################################################

case "$DIE" in
	[0-3]*)
		echo Die $DIE
		;;
	*)
		echo Invalid Die: $DIE
		exit 1
		;;
esac

case "$LEAF" in
        [0-1]*)
          echo pciecore $LEAF
                ;;
        *)
          echo Invalid Leaf: $LEAF
          exit
                ;;
esac

case "$PORT" in
        [0-7]*)
          echo Port $PORT
                ;;
        *)
          echo Invalid Port: $PORT
          exit
                ;;
esac
echo input arg end .........
echo
#NODE其实是DIE的绝对值，这个这里的DIE是一个SOCKET中DIE的相对值
NODE=$[$DIE + $[$SOCKET * 4]]
#ROOT=$(hex $[$NODE * 0x10])
# Read Die1 sub-bus number 
echo NODE=$NODE
#通过IOHC1的DF 00:19.0的DF获取IOHC的bus，然后乘以NODE
ROOT=$(hex $[$NODE * 0x$(setpci -s 0:19.0 84.B)])
echo NBIOBUS=$ROOT
#获取该port对应的IOHC的bdf号
ROOTDEV=$ROOT:0.0
#这里可能是想获取DF
DEVICE=$(hex $[0x18 + $NODE])
PCIEDEV=$DEVICE.6

echo ++++IOHCDEV=$ROOTDEV
#echo ++++PCIE=$PCIEDEV

# clear SMN access registers
#################################################
echo "clear SMN access registers"
echo "setpci -s $PCIEDEV 48.L=0"
echo "setpci -s $PCIEDEV 58.L=0"

setpci -s $PCIEDEV 48.L=0
setpci -s $PCIEDEV 58.L=0

#################################################

# PCIE PORT SMN base
PCIEBASE=0x$(hex $[0x11140000 + $[$LEAF * 0x00100000] + $[$PORT * 0x00001000]])
echo ++++PCIECORE$LEAF PORT$PORT SMN base addr =$PCIEBASE
PCIE_ERR_CNTL=0x$(hex $[$PCIEBASE + 0x1A8])
echo ++++PCIECORE$LEAF PORT$PORT PCIEPORT::PCIE_ERR_CNT SMN addr =$PCIE_ERR_CNTL

# IOHC base for PCIE port
IOHCBASE=0x$(hex $[0x13B01000])
IOHC_SUB_BUS_NUMBER_LATENCY=0x$(hex $[$IOHCBASE + $[$LEAF * 0x00000800] + $[$PORT * 0x00000100] + 0x818])
echo ++++PCIECORE$LEAF PORT$PORT IOHC Sub bus num SMN addr =$IOHC_SUB_BUS_NUMBER_LATENCY
echo 

if [ "$TYPE" == "lcrc-rx" ]; then
	ENABLE_SHIFT=5
elif [ "$TYPE" == "lcrc-tx" ]; then
	ENABLE_SHIFT=4
elif [ "$TYPE" == "acs-fatal" ]; then
	ENABLE_SHIFT=0
elif [ "$TYPE" == "acs-nonfatal" ]; then
	ENABLE_SHIFT=0
elif [ "$TYPE" == "ecrc-rx" ]; then
	ENABLE_SHIFT=7
elif [ "$TYPE" == "ecrc-tx" ]; then
    ENABLE_SHIFT=6
else
	echo "Invalid TYPE: $TYPE"
	exit 1
fi

# Setting up trap handler to clean up if interrupted
trap "cleanup $ROOTDEV" SIGINT SIGQUIT SIGHUP

# Unmask corrected or non-fatal advisory errors
if [ "$TYPE" == "lcrc-rx" ] || [ "$TYPE" == "lcrc-tx" ] || [ "$TYPE" == "ecrc-tx" ] || [ "$TYPE" == "ecrc-rx" ]; then
	MASK=0x$(hex $[0xFFFFBFFF])
elif [ "$TYPE" == "acs-fatal" ] || [ "$TYPE" == "acs-nonfatal" ]; then
	MASK=0x$(hex $[0xFFDFFFFF])
fi

if [ "$TYPE" == "lcrc-rx" ] || [ "$TYPE" == "lcrc-tx" ] || [ "$TYPE" == "ecrc-tx" ] || [ "$TYPE" == "ecrc-rx" ]; then
	# Unmask corrected errors
	echo ++++++++++++++++++++++++++++begin unmask correct error+++++++++++++++++++++++++++++++++++++++++++++++++
	PCIE_CORR_ERR_MASK=0x$(hex $[$PCIEBASE + 0x0164 - 0x40000])
	#通过SMN INDE清除pcie aer的mask
	echo use SMN read PCIECORE$LEAF PORT$PORT aer cap correct err mask,cmd: setpci -s $ROOTDEV 60.L=$PCIE_CORR_ERR_MASK
	setpci -s $ROOTDEV 60.L=$PCIE_CORR_ERR_MASK
	PCIE_CORR_ERR_MASK_VAL=$(setpci -s $ROOTDEV 64.L)
	echo PCIECORE$LEAF PORT$PORT aer cap correct err mask SMN addr $PCIE_CORR_ERR_MASK value $PCIE_CORR_ERR_MASK_VAL
	echo
	
	PCIE_CORR_ERR_MASK_NEWVAL=0x$(hex $[0x$PCIE_CORR_ERR_MASK_VAL & $MASK])
	echo use SMN index PCIECORE$LEAF PORT$PORT PCIEPORT::PCIE_ERR_CNT reg, cmd:setpci -s $ROOTDEV 60.L=$PCIE_CORR_ERR_MASK
	setpci -s $ROOTDEV 60.L=$PCIE_CORR_ERR_MASK
	echo write $PCIE_CORR_ERR_MASK_NEWVAL to PCIECORE$LEAF PORT$PORT aer cap correct err mask reg, cmd: setpci -s $ROOTDEV 64.L=$PCIE_CORR_ERR_MASK_NEWVAL
	setpci -s $ROOTDEV 64.L=$PCIE_CORR_ERR_MASK_NEWVAL
	
	echo echo ++++++++++++++++++++++++++++end unmask correct error+++++++++++++++++++++++++++++++++++++++++++++++++
	echo
	
	echo ++++++++++++++++++++++++++++begin set pci err cntl correct error+++++++++++++++++++++++++++++++++++++++++++++++++
	# Set up PCI_ERR_CNTL value
	PCIE_ERR_CNTL_VAL=0x$(hex $[(0x1 << ENABLE_SHIFT)])

	# Set PCI_ERR_CNTL
	echo use SMN index PCIECORE$LEAF PORT$PORT PCIEPORT::PCIE_ERR_CNT reg, cmd:setpci -s $ROOTDEV 60.L=$PCIE_ERR_CNTL
	setpci -s $ROOTDEV 60.L=$PCIE_ERR_CNTL
	echo wrie $PCIE_ERR_CNTL_VAL to PCIECORE$LEAF PORT$PORT PCIEPORT::PCIE_ERR_CNT reg,cmd:setpci -s $ROOTDEV 64.L=$PCIE_ERR_CNTL_VAL
	setpci -s $ROOTDEV 64.L=$PCIE_ERR_CNTL_VAL
    echo ++++++++++++++++++++++++++++end set pci err cntl correct error+++++++++++++++++++++++++++++++++++++++++++++++++
	echo
	# Try and get some traffic going
	lspci > /dev/null

elif [ "$TYPE" == "acs-fatal" ] || [ "$TYPE" == "acs-nonfatal" ]; then
	# Check ACS enablement in PCIe
	PCIE_ACS_CNTL=0x$(hex $[$PCIEBASE + 0x2A6 - 0x40000])

	setpci -s $ROOTDEV 60.L=$PCIE_ACS_CNTL
	PCIE_ACS_CNTL_VAL=$(setpci -s $ROOTDEV 64.L)

	ACS_EN=$(( (0x$(hex 0x$PCIE_ACS_CNTL_VAL) & (0x1 << $ENABLE_SHIFT)) ))
	if (( $ACS_EN == 0 )); then
		echo "ACS not enabled; exiting"
		cleanup $ROOTDEV
		exit 1
	fi

	# Read SUB_BUS_NUMBER_LATENCY register from PCIe
	PCIE_SUB_BUS_NUMBER_LATENCY=0x$(hex $[$PCIEBASE + 0x018 - 0x40000])
	setpci -s $ROOTDEV 60.L=$PCIE_SUB_BUS_NUMBER_LATENCY
	PCIE_SUB_BUS_NUMBER_LATENCY_VAL=$(setpci -s $ROOTDEV 64.L)

	# Unmask ACS violation errors
	PCIE_UNCORR_ERR_MASK=0x$(hex $[$PCIEBASE + 0x0158 - 0x40000])
	setpci -s $ROOTDEV 60.L=$PCIE_UNCORR_ERR_MASK
	PCIE_UNCORR_ERR_MASK_VAL=$(setpci -s $ROOTDEV 64.L)
	PCIE_UNCORR_ERR_MASK_NEWVAL=0x$(hex $[0x$PCIE_UNCORR_ERR_MASK_VAL & $MASK])
	setpci -s $ROOTDEV 60.L=$PCIE_UNCORR_ERR_MASK
	setpci -s $ROOTDEV 64.L=$PCIE_UNCORR_ERR_MASK_NEWVAL

        PCIE_UNCORR_ERR_SEVERITY=0x$(hex $[$PCIEBASE + 0x015C - 0x40000])
        setpci -s $ROOTDEV 60.L=$PCIE_UNCORR_ERR_SEVERITY
        PCIE_UNCORR_ERR_SEVERITY_VAL=$(setpci -s $ROOTDEV 64.L)

	if [ "$TYPE" == "acs-nonfatal" ]; then
		# Clear bit 21 of PCIE_UNCORR_ERR_SEVERITY
		PCIE_UNCORR_ERR_SEVERITY_NEWVAL=0x$(hex $[0x$PCIE_UNCORR_ERR_SEVERITY_VAL & $MASK])
	elif [ "$TYPE" == "acs-fatal" ]; then
		# Set bit 21 of PCIE_UNCORR_ERR_SEVERITY
                PCIE_UNCORR_ERR_SEVERITY_NEWVAL=0x$(hex $[0x$PCIE_UNCORR_ERR_SEVERITY_VAL | (~$MASK & 0xFFFFFFFF)])
	else
		echo "Bad Type $TYPE"
		cleanup $ROOTDEV
		exit 1
	fi
	# Set the severity
	setpci -s $ROOTDEV 60.L=$PCIE_UNCORR_ERR_SEVERITY
	setpci -s $ROOTDEV 64.L=$PCIE_UNCORR_ERR_SEVERITY_NEWVAL

	# Set SUB_BUS_NUM to 0xFF
	PCIE_HEXMASK=0x$(hex $[0x00FFFF00])
	PCIE_SUB_BUS_NUMBER_LATENCY_NEWVAL=0x$(hex $[0x$PCIE_SUB_BUS_NUMBER_LATENCY_VAL | $PCIE_HEXMASK])
	
	setpci -s $ROOTDEV 60.L=$PCIE_SUB_BUS_NUMBER_LATENCY
	setpci -s $ROOTDEV 64.L=$PCIE_SUB_BUS_NUMBER_LATENCY_NEWVAL

	# Reset the IOHC SHADOW copy of this register
	setpci -s $ROOTDEV 60.L=$IOHC_SUB_BUS_NUMBER_LATENCY
	setpci -s $ROOTDEV 64.L=$PCIE_SUB_BUS_NUMBER_LATENCY_VAL

	# Wait a bit and hope for DMA, and then reset the register
	sleep 1

	# Reset so we aren't permanently borked
        setpci -s $ROOTDEV 60.L=$PCIE_SUB_BUS_NUMBER_LATENCY
        setpci -s $ROOTDEV 64.L=$PCIE_SUB_BUS_NUMBER_LATENCY_VAL
 
fi

# Clean up
cleanup $ROOTDEV

exit
