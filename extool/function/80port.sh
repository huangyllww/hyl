################################################################
###
### Print 80 port
###
### Parameter: $1 = Function name
### 
################################################################


#included files

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