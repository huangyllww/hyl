################################################################
###
### Search bios setup value
###
### Parameter: none
### 
################################################################


#included files

function SearchBIOSConf()
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
    ./sistool getconf bios -H $bmcip -U $bmcusr -P $bmcpw -O sistlogs/SearchBIOSConfResult > /dev/null
    cat sistlogs/SearchBIOSConfResult|grep -i $1 -B1
    rm sistlogs/SearchBIOSConfResult
    cd $tmp_path
}
