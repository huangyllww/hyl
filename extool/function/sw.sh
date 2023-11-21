################################################################
###
### Switch to another environment
###
### Parameter: $1 = new ip
### 
################################################################


#included files
source $LIB_DIR/_autobmcpw.sh
source $LIB_DIR/_CheckIpValid.sh

function sw() {
    SwitchEnvironment $1
}

function _SwitchToInputIP()
{
    local CurrentLineNum
    local CurrentLineContext
    local CurrentGroupNum
    local iTmp
    local VarName
    local VarValue
    local globalGroupNumContext
    
    CurrentLineNum=$1
    #更新密码
    _autobmcpw $CurrentLineNum
    
    CurrentLineContext=$(cat $CONF_DIR/EnvConfExt | sed -n "$CurrentLineNum,$CurrentLineNum p") 

    #获取当前组号
    CurrentGroupNum=$(echo $CurrentLineContext | awk -F'#:bmcip' '{print $1}')
    CurrentGroupNum=$(echo $CurrentGroupNum | awk -F'#' '{print $2}')

    #更新环境和配置文件
    iTmp=1
    while [ $iTmp -le 6 ]
    do
        #清除前四个字符
        CurrentLineContext=${CurrentLineContext#*'#:'} 
        #export
        VarName=$(echo $CurrentLineContext | awk -F'=' '{print $1}')
        VarValue=$(echo $CurrentLineContext | awk -F'=' '{print $2}')
        export $VarName=$VarValue

        # iTmp++; CurrentLineOfBmcip++;
        iTmp=$[1+$iTmp]
        CurrentLineNum=$[1+$CurrentLineNum]
        CurrentLineContext=$(cat $CONF_DIR/EnvConfExt | sed -n "$CurrentLineNum,$CurrentLineNum p")
    done 
    
    #清除数组,更新全局变量globalGroupNum，结束搜索
    globalGroupNum=$CurrentGroupNum
    globalGroupNumContext="globalGroupNum=$globalGroupNum"
    sed -in "1c "$globalGroupNumContext"" $CONF_DIR/EnvConfExt

    #dump ip信息
    if [[ "$bmcusr" == "unknown" ]]
    then 
        echo -e "\e[1;33mBMC IP: \e[0m\e[1;33m$bmcip\t\e[0m\e[1;30mBMC USR:\e[0m\e[1;30m$bmcusr\t\e[0m\e[1;30mBMC PW:\e[0m\e[1;30m$bmcpw\e[0m"
    else
        echo -e "\e[1;33mBMC IP: \e[0m\e[1;33m$bmcip\t\e[0m\e[1;30mBMC USR:\e[0m\e[1;33m$bmcusr\t\e[0m\e[1;30mBMC PW:\e[0m\e[1;33m$bmcpw\e[0m"
    fi

    if [[ -z "$osip" ]]
    then 
        echo -e "\e[1;36m OS IP: \e[0m\e[1;36m$osip\t\t\e[0m\e[1;30m OS USR:\e[0m\e[1;36m$osusr\t\e[0m\e[1;30m OS PW:\e[0m\e[1;36m$ospw\e[0m"
    else
        echo -e "\e[1;36m OS IP: \e[0m\e[1;36m$osip\t\e[0m\e[1;30m OS USR:\e[0m\e[1;36m$osusr\t\e[0m\e[1;30m OS PW:\e[0m\e[1;36m$ospw\e[0m"
    fi
    return 0
}

function SwitchEnvironment()
{
    local InputIpArray=(10 8 140 255)
    local GroupTagArray
    local LineOfFindArray
    local FindIpOnlyArray
    local SerchTagStr
    local iTmp
    
    # GROUP方式切换
    # 判断输入的数据是不是 "#num" 格式
    GroupTagArray=($(cat $CONF_DIR/EnvConfExt | grep -n "$1#:bmcip=" | awk -F':' '{print $1}'))
    if [[ ${#GroupTagArray[@]} -eq 1 ]]
    then
        _SwitchToInputIP ${GroupTagArray[1]}
        return 0
    fi

    # 将输入的信息处理成数组
    _CheckIpValid $1

    # 数组赋值
    str="$1"
    InputIpArray=(${=${str//./ }})  #分割符是“ ”赋值给array 就成了数组赋值

    # 将输入的数据组装成可搜索的tag
    iTmp=1    
    while [ $iTmp -ge 1 -a $iTmp -le ${#InputIpArray[@]} ]
    do
        SerchTagStr=$SerchTagStr"\.${InputIpArray[iTmp]}"
        iTmp=$[$iTmp+1]
    done

    # 根据上述的tag, 在配置文件中搜索相应字串, 将搜索结果存放在数组中
    LineOfFindArray=($(cat $CONF_DIR/EnvConfExt | grep -n "#:bmcip=" | grep "${SerchTagStr}$" | awk -F':' '{print $1}'))

    # 分析数组成员个数, 做出相应处理
    if [[ ${#LineOfFindArray[@]} -eq 0 ]]
    then
        echo "------------------------" 
        echo "no such ip!!"
        echo "------------------------" 
    elif [[ ${#LineOfFindArray[@]} -eq 1 ]]
    then
        # 直接切换到该ip
        CurrentLineNum=${LineOfFindArray[1]}
        _SwitchToInputIP $CurrentLineNum
    else
        # 处理数据
        iTmp=1
        while [ $iTmp -ge 1 -a $iTmp -le ${#LineOfFindArray[@]} ]
        do
            CurrentLineNum=${LineOfFindArray[iTmp]}
            FindIpOnlyArray[$iTmpi]=$(cat $CONF_DIR/EnvConfExt | sed -n "$CurrentLineNum,$CurrentLineNum p")
            iTmp=$[$iTmp+1]
        done

        # 用户选择后再切换
        echo "检测到多个搜索结果: "
        select opt in "${FindIpOnlyArray[@]}";
            case $opt in
                *) 
                CurrentLineNum=$(cat $CONF_DIR/EnvConfExt | grep -n "$opt" | awk -F':' '{print $1}')
                _SwitchToInputIP $CurrentLineNum
                break
                ;;
            esac
    fi
}
