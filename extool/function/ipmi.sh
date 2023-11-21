################################################################
###
### ipmitool fast instruction 
###
### Parameter: none
### 
################################################################


#included files

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

function bootdev()
{
    
    dumpstring='Host IP:'$bmcip"\n"
    ##Set Debug Mode
    AccordedTime=$(date "+%Y-%m-%d-%H:%M:%S")
    dumpstring=$dumpstring$*
    dumpstring=$dumpstring" ("$AccordedTime")"
    echo -e $dumpstring
    echo "----------------"
    ipmitool -I lanp -H $bmcip -U $bmcusr -P $bmcpw chassis bootdev $*
}

function power()
{
    
    dumpstring='Host IP:'$bmcip"\n"
    ##Set Debug Mode
    AccordedTime=$(date "+%Y-%m-%d-%H:%M:%S")
    dumpstring=$dumpstring$*
    dumpstring=$dumpstring" ("$AccordedTime")"
    echo -e $dumpstring
    echo "----------------"
    ipmitool -I lanp -H $bmcip -U $bmcusr -P $bmcpw chassis power $*
}
