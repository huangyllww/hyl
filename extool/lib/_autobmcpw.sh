################################################################
###
### Modified variable
###
### Parameter: $1 = bmcip
### 
################################################################


#included files

function _autobmcpw()
{
    CurrentLine=$1
    if [[ -z "$CurrentLine" ]]
    then
        return 0
    fi

    #更新bmc 用户名和密码
    #找到当前bmcip/bmcusr/bmcpw
    CurrentIp=$(sed -n "$[$CurrentLine]"p $CONF_DIR/EnvConfExt | awk -F'bmcip=' '{print $2}')
    OldUsr=$(sed -n "$[$CurrentLine+1]"p $CONF_DIR/EnvConfExt | awk -F'bmcusr=' '{print $2}')
    OldPw=$(sed -n "$[$CurrentLine+2]"p $CONF_DIR/EnvConfExt | awk -F'bmcpw=' '{print $2}')
    #echo -e "$OldUsr"
    #echo -e "$OldPw"

    if [[ -z "$CurrentIp" ]]
    then
        return 0
    fi
    
    # 过滤掉ping不通的ip
    PingCheckVal=0
    #五ping校验
    ping -c 1 -W 0.05 $CurrentIp  > /dev/null 2>&1
    PingCheckVal=$[$?+$PingCheckVal]
    ping -c 1 -W 0.05 $CurrentIp  > /dev/null 2>&1
    PingCheckVal=$[$?+$PingCheckVal]
    ping -c 1 -W 0.05 $CurrentIp  > /dev/null 2>&1
    PingCheckVal=$[$?+$PingCheckVal]
    ping -c 1 -W 0.05 $CurrentIp  > /dev/null 2>&1
    PingCheckVal=$[$?+$PingCheckVal]
    ping -c 1 -W 0.05 $CurrentIp  > /dev/null 2>&1
    PingCheckVal=$[$?+$PingCheckVal]

    if [ $PingCheckVal -eq 5 ]
    then
        return 0
    fi
    
    # 判断是否超时，如果超时则继续
    cat /dev/null > $CONF_DIR/null
    timeout 1 ipmitool -I lanplus -H $CurrentIp -U $OldUsr -P $OldPw  Chassis Power > $CONF_DIR/null 2>&1
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
                    sed -rin ""$[$CurrentLine+1],$[$CurrentLine+1]"s/"$OldUsr"/"$username"/g" $CONF_DIR/EnvConfExt
                    sed -rin ""$[$CurrentLine+2],$[$CurrentLine+2]"s/"$OldPw"/"$passward"/g" $CONF_DIR/EnvConfExt
                    #sed -rin ""$[$CurrentLine+1]"s/.*/"=$username"/" $CONF_DIR/EnvConfExt
                    #sed -rin ""$[$CurrentLine+2]"s/"$OldPw"/"$passward"/g" $CONF_DIR/EnvConfExt
                    bmcusr=$username
                    bmcpw=$passward
                    return 0 
                fi
            fi
        fi
    done  < $CONF_DIR/AutoFixVarList
    # 密码变更结束
    # 都找不到则更新为unknown
    sed -rin ""$[$CurrentLine+1],$[$CurrentLine+1]"s/$OldUsr/unknown/g" $CONF_DIR/EnvConfExt
    sed -rin ""$[$CurrentLine+2],$[$CurrentLine+2]"s/$OldPw/unknown/g" $CONF_DIR/EnvConfExt
    bmcusr=$username
    bmcpw=$passward
    return 0
}

