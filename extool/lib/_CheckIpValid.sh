################################################################
###
### Verify that the entered parameters are valid 
###
### Parameter: $1=IP
### 
################################################################


#included files
source $LIB_DIR/_PrintErrorInfo.sh

function _CheckIpValid()  
{
    local DefaultIpArray=(10 8 140 255)
    local NewIpArray=(0)
    local NewArrayNum=0
    local ValidFlag=0
    local i=0 j=4

    # 数组赋值
    str="$1"
    NewIpArray=(${=${str//./ }})  #分割符是“ ”赋值给array 就成了数组赋值
    # echo ${NewIpArray[@]}

    NewArrayNum=${#NewIpArray[@]}
    # echo 'NewArrayNum='$NewArrayNum

    i=$NewArrayNum
    #校验输入ip是否合法
    while [ $i -ge 1 -a $i -le $NewArrayNum ]
    do
        if [ ${NewIpArray[i]} -gt 0 -a ${NewIpArray[i]} -lt 255 ]
        then
            ValidFlag=$[$ValidFlag+1]
        fi
        i=$[$i-1]
    done

    if [ $ValidFlag -eq $NewArrayNum ]
    then
        i=$NewArrayNum
        j=4
        while [ $i -ge 1 -a $i -le $NewArrayNum ]
        do
            DefaultIpArray[j]=${NewIpArray[i]}
            j=$[$j-1]
            i=$[$i-1]
        done
        NewIp=${DefaultIpArray[1]}.${DefaultIpArray[2]}.${DefaultIpArray[3]}.${DefaultIpArray[4]}
        return 1
    else
        _PrintErrorInfo
    fi
}
