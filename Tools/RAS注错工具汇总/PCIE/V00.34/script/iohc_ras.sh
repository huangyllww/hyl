#!/bin/bash
#######################################
# 清理或者读取RAS action相关reg
# Globals:
#   global1
#   global2
# Arguments:
#   argument1  逻辑dieid
#   argument2  read或者write标志
# Returns:
#   return1
#######################################
hex()
{
	echo $(printf "%x" $1)
}

sum=0
IOHC_RAS_BASE=0x13B20168
OFFSET=0x4
IOHC_RAS_END=0x13B20378
A=`whoami`

if [ $A != 'root' ]; then
   echo "You have to be root to run this script"
   exit 1;
fi
cat /proc/cmdline
Die=$1
ACTION=$2

echo "Command line arguments:"
echo "argument1: logical dieid, argument2: action"
echo "dieid is the logical die number"
echo "action only support read or write"
echo ""
if [ "$ACTION" == "read" ]; then
	echo "-----------------------$ACTION Die $Die IHOC RAS Register---------------------"
	echo "---------------SMN Base Addr $IOHC_RAS_BASE End Addr $IOHC_RAS_END---------------"
	for((smn_addr = 0x$(hex $[$IOHC_RAS_BASE]); smn_addr <= 0x$(hex $[IOHC_RAS_END]);))
		do
			./readsmn $Die $smn_addr
			let "smn_addr+=OFFSET"
		done
	echo "--------------end $ACTION IOHC RAS Register----------------"
elif [ "$ACTION" == "write" ]; then
	echo "-----------------------$ACTION Die $Die IHOC RAS Register---------------------"
	echo "---------------SMN Base Addr $IOHC_RAS_BASE End Addr $IOHC_RAS_END---------------"
	for((smn_addr = 0x$(hex $[$IOHC_RAS_BASE]); smn_addr <= 0x$(hex $[IOHC_RAS_END]);))
		do
			./writesmn $Die $smn_addr 0
			let "smn_addr+=OFFSET"
		done
	echo "--------------end $ACTION IOHC RAS Register----------------"
else
echo "Bad action $ACTION, only support read or write"
	exit 1
fi
#lspci -vvvv -xxxx

