################################################################
###
### Modified variable
###
### Parameter: $1 = osip
### 
################################################################


#included files

function _autoospw()
{
    local FixBmcArray=()

    #整理配置文件内容格式
    #找#:bmcip= ，没有则从第一行开始
    #首先筛选出全部bmcip所在行数，并赋值给变量
    FixBmcArray=($(awk "/#:bmcip=$1/{print NR}" $CONF_DIR/EnvConfExt))

    #将数组 FixBmcArray()的第一个元素, 作为搜索行, 第一个元素为 null 则结束
    FixBmcPwCurrentLine=${FixBmcArray[1]}
    if [[ -z "$FixBmcPwCurrentLine" ]]
    then
        return 0
    fi

    #更新bmc 用户名和密码
    #找到当前bmcip/bmcusr/bmcpw
    CurrentIp=$(sed -n "$[$FixBmcPwCurrentLine]"p $CONF_DIR/EnvConfExt | awk -F'bmcip=' '{print $2}')
    OldBmcUsr=$(sed -n "$[$FixBmcPwCurrentLine+1]"p $CONF_DIR/EnvConfExt | awk -F'bmcusr=' '{print $2}')
    OldBmcPw=$(sed -n "$[$FixBmcPwCurrentLine+2]"p $CONF_DIR/EnvConfExt | awk -F'bmcpw=' '{print $2}')

    if [[ -z "$CurrentIp" ]]
    then
        return 0
    fi
    
    # 过滤掉ping不通的ip
    PingCheckVal=0
    #三ping校验
    ping -c 1 -W 0.05 $CurrentIp  > /dev/null 2>&1
    PingCheckVal=$[$?+$PingCheckVal]
    ping -c 1 -W 0.05 $CurrentIp  > /dev/null 2>&1
    PingCheckVal=$[$?+$PingCheckVal]
    ping -c 1 -W 0.05 $CurrentIp  > /dev/null 2>&1
    PingCheckVal=$[$?+$PingCheckVal]

    if [ $PingCheckVal -eq 3 ]
    then
        return 0
    fi
    
    # 判断是否超时，如果超时则继续
    cat /dev/null > $CONF_DIR/null
    timeout 1 ipmitool -I lanplus -H $CurrentIp -U $OldBmcUsr -P $OldBmcPw  Chassis Power > $CONF_DIR/null 2>&1
    if [ $? -eq 124 ]; then
        return 0
    fi

    # 判断原密码是否有效, 有效则跳过
    result=$(cat $CONF_DIR/null | grep -i "Chassis Power")
    if [[ -n "$result" ]]
    then
        return 0
    fi
    
    #旧密码无效, 需要变更
    while read line;
    do 
        if [ -z "$line" ]
        then
            break
        else
            username=$(echo $line | awk -F'=' ' { print $1 }')
            passward=$(echo $line | awk -F'=' ' { print $2 }')   
            valid=$(echo $line | awk -F'=' ' { print $3 }')   
            if [[ -n "$username" ]] && [[ -n "$passward" ]] && [[ -z "$valid" ]]
            then
                #清空null文件内容,避免信息混淆
                cat /dev/null > $CONF_DIR/null
                # usr/pw 校验通过, 可以进行连通性测试
                timeout 1 ipmitool -I lanplus -H $CurrentIp -U $username -P $passward  chassis power > $CONF_DIR/null 2>&1
                result=$(cat $CONF_DIR/null | grep -i "Chassis Power")
                if [[ -n "$result" ]]
                then
                    #这里 bmc 的 usr/pw 已经准备好了, 需要写入配置文件 EnvConfExt
                    sed -rin ""$[$FixBmcPwCurrentLine+1],$[$FixBmcPwCurrentLine+1]"s/$OldBmcUsr/$username/g" $CONF_DIR/EnvConfExt
                    sed -rin ""$[$FixBmcPwCurrentLine+2],$[$FixBmcPwCurrentLine+2]"s/$OldBmcPw/$passward/g" $CONF_DIR/EnvConfExt
                    break 
                fi
                
            fi
        fi
    done  < $CONF_DIR/AutoFixVarList
    # 密码变更结束
}

