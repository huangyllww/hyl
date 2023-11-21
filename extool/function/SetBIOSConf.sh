################################################################
###
### set bios item
###
### Parameter: none
### 
################################################################


#included files

function SetBIOSConf()
{
    
    dumpstring='Host IP:'$bmcip"\n"
    ##Set Debug Mode
    AccordedTime=$(date "+%Y-%m-%d-%H:%M:%S")
    dumpstring=$dumpstring$*
    dumpstring=$dumpstring" ("$AccordedTime")"
    echo -e $dumpstring
    echo "----------------"
    
    tmp_path=`pwd`
    cd $TOOL_DIR
    ./sistool setconf bios -H $bmcip -U $bmcusr -P $bmcpw -A $1 -V $2
    cd $tmp_path
}