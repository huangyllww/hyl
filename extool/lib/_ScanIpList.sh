################################################################
###
### Show each IP from config file, and return ping status 
###
### Parameter: $1 = IP tag (bmcip or osip)
### 
################################################################


#included files

function _ScanIpList()  
{
    if [[ "$1" == "bmcip" ]] || [[ "$1" == "osip" ]]
    then
        #扫描全部IP
        if [[ "$1" == "bmcip" ]]
        then 
            tag='\e[1;33mBMC IP\e[0m'
        else
            tag="\e[1;36mOS IP\e[0m"
        fi
        
        r=($(cat $CONF_DIR/EnvConfExt | grep "$1="))
        i=1
        echo -e "\e[0m\e[1;30m-------------------------------\e[0m"
        echo -e "\e[1;31mGROUP\e[0m\t$tag\t\tSTATUS"
        echo -e "\e[0m\e[1;30m-------------------------------\e[0m"
        # 定义变量

##################################################################################
#    url="https://www.baidu.com"
#    link_text="Click here to visit our website!"
#
#    # 显示超链接
#    echo -e "\033[1m$link_text\033[0m"
#
#    # 等待用户点击
#    read -nl -r -p "Press enter to visit our website..."
#
#    # 打开网站
#    echo -e $url
##################################################################################
        while [ $i -le ${#r[*]} ]
        do
            IpScanCurVar=${r[$i]}

            IpOnly=$(echo $IpScanCurVar | awk -F'ip=' '{print $2}')

            #add color
            if [[ "$1" == "bmcip" ]]
            then 
                IpColor="\e[1;33m$IpOnly\e[0m"
            else
                IpColor="\e[1;36m$IpOnly\e[0m"
            fi

            #group
            Group="\e[1;30m#\e[0m\e[1;31m${$(echo $IpScanCurVar | awk -F"#:$1=" '{print $1}'):1}\e[0m\e[1;30m#\e[0m"

            #delete null ip
            if [[ -n "$IpOnly" ]]
            then
                PingCheckVal=0
                #五ping校验
                ping -c 1 -W 0.05 $IpOnly  > $CONF_DIR/null 2>&1
                PingCheckVal=$[$?+$PingCheckVal]
                ping -c 1 -W 0.05 $IpOnly  > $CONF_DIR/null 2>&1
                PingCheckVal=$[$?+$PingCheckVal]
                ping -c 1 -W 0.05 $IpOnly  > $CONF_DIR/null 2>&1
                PingCheckVal=$[$?+$PingCheckVal]
                ping -c 1 -W 0.05 $IpOnly  > $CONF_DIR/null 2>&1
                PingCheckVal=$[$?+$PingCheckVal]
                ping -c 1 -W 0.05 $IpOnly  > $CONF_DIR/null 2>&1
                PingCheckVal=$[$?+$PingCheckVal]

                if [ $PingCheckVal -lt 5 ]; then
                    echo -e " $Group\t$IpColor\t\e[0m\e[1;32mONLINE\e[0m"
                else
                    echo -e " $Group\t$IpColor\t\e[0m\e[1;30mOFFLINE\e[0m"
                fi
            fi

            #i++
            i=$[$i+1]
        done
        echo -e "\e[0m\e[1;30m-------------------------------\e[0m"
        return 0
    fi
}
