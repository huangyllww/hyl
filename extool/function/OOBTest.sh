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
