#! /bin/bash
#source OemScript.sh

BASE_DIR=$(pwd)
LIB_DIR=$BASE_DIR'/lib'
TOOL_DIR=$BASE_DIR'/tool'
CONF_DIR=$BASE_DIR'/conf'
FUNCTION_DIR=$BASE_DIR'/function'

source $LIB_DIR/_FirstStartupInit.sh
_FirstStartupInit
source $LIB_DIR/OemScript.sh


#function list
source $FUNCTION_DIR/0help.sh
source $FUNCTION_DIR/80port.sh
# source $FUNCTION_DIR/Jumper.sh
source $FUNCTION_DIR/SearchBIOSConf.sh
source $FUNCTION_DIR/SetBIOSConf.sh
source $FUNCTION_DIR/autolog.sh
source $FUNCTION_DIR/biosfwupdate.sh
source $FUNCTION_DIR/bmclogin.sh
source $FUNCTION_DIR/ihdt.sh
source $FUNCTION_DIR/debugmode.sh
source $FUNCTION_DIR/iplist.sh
source $FUNCTION_DIR/ipmi.sh
source $FUNCTION_DIR/oslogin.sh
source $FUNCTION_DIR/sw.sh
source $FUNCTION_DIR/varset.sh