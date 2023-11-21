################################################################
###
### Automatic log generation
###
### Parameter: none
### 
################################################################


#included files

function autolog()
{
    dumpstring='Host IP:'$bmcip"\n"
    AccordedTime=$(date "+%Y-%m-%d-%H:%M:%S")
    dumpstring=$dumpstring" ("$AccordedTime")" 

    if [ $# -eq 0 ]
    then
        LogName=$(date "+%Y%m%d_%H%M%S")
    elif [ $# -eq 1 ]
    then
        

		case $1 in
			"showonly")
				#判断是否输入参数，例如输入有效文件名
				echo -e $dumpstring
				echo "----------------"
				while [[ 1 ]] ; do ipmitool -I lanplus -U $bmcusr -P $bmcpw -H $bmcip sol deactivate; ipmitool -I lanplus -U $bmcusr -P $bmcpw -H $bmcip sol activate; sleep 2; done
				return 0
				;;
			*)
                LogName=$1
                ;;
		esac
    else
        echo "------------------------"
        echo "Invalid file name!!"
        echo "------------------------" 
        return 0
    fi

    #判断是否输入参数，例如输入有效文件名
    echo -e $dumpstring
    dumpstring="log Path:$BASE_DIR/autolog/$bmcip/"$LogName'.log'"\n"
    #检查并创建文件夹
    if [ ! -d "$BASE_DIR/autolog/$bmcip/" ];
    then
        mkdir $BASE_DIR/autolog/$bmcip
    fi
    
    echo -e $dumpstring
    echo "----------------"
	while [[ 1 ]] ; do ipmitool -I lanplus -U $bmcusr -P $bmcpw -H $bmcip sol deactivate; ipmitool -I lanplus -U $bmcusr -P $bmcpw -H $bmcip sol activate; sleep 2; done | tee $BASE_DIR/autolog/$bmcip/$LogName.log
    return 0
}
