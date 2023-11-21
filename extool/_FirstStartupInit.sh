################################################################
### 
### Initializes the environment at first startup
###
### Parameter: none
### 
################################################################


#included files
source $LIB_DIR/_ReorderVar.sh
source $LIB_DIR/_autobmcpw.sh

function _FirstStartupInit()
{
    #重排
    _ReorderVar
    #chmod 777 $TOOL_DIR/*
    
    #读取组数
    globalGroupNumContext=$(cat $CONF_DIR/EnvConfExt | sed -n "1,1p")
    globalGroupNum=$(echo $globalGroupNumContext | awk -F'globalGroupNum=' '{print $2}')
    if [[ $globalGroupNum -eq 0 ]]
    then
        #输出错误提示信息
        echo "------------------------"
        echo "Invalid globalGroupNum !!"
        echo "------------------------"
    else
        #初始化环境变量

        #找到组内容
        InitEnvBmcipTag="#$globalGroupNum"'#:bmcip'
        InitEnvBmcusrTag="#$globalGroupNum"'#:bmcusr'
        InitEnvBmcpwTag="#$globalGroupNum"'#:bmcpw'
        InitEnvOsipTag="#$globalGroupNum"'#:osip'
        InitEnvOsusrTag="#$globalGroupNum"'#:osusr'
        InitEnvOspwTag="#$globalGroupNum"'#:ospw'

        #获取行号
        InitEnvBmcipLine="$(sed -n "/$InitEnvBmcipTag/=" $CONF_DIR/EnvConfExt)"
        InitEnvBmcusrLine="$(sed -n "/$InitEnvBmcusrTag/=" $CONF_DIR/EnvConfExt)"
        InitEnvBmcpwLine="$(sed -n "/$InitEnvBmcpwTag/=" $CONF_DIR/EnvConfExt)"
        InitEnvOsipLine="$(sed -n "/$InitEnvOsipTag/=" $CONF_DIR/EnvConfExt)"
        InitEnvOsusrLine="$(sed -n "/$InitEnvOsusrTag/=" $CONF_DIR/EnvConfExt)"
        InitEnvOspwLine="$(sed -n "/$InitEnvOspwTag/=" $CONF_DIR/EnvConfExt)"

        #校验行号
        InitLineValid1=$[3+$InitEnvBmcipLine]
        InitLineValid2=$[3+$InitEnvBmcusrLine]
        InitLineValid3=$[3+$InitEnvBmcpwLine]
        if [ $InitLineValid1 -eq $InitEnvOsipLine -a $InitLineValid2 -eq $InitEnvOsusrLine -a $InitLineValid3 -eq $InitEnvOspwLine ]
        then
            #初始化bmcip
            InitEnvBmcipCont=$(cat $CONF_DIR/EnvConfExt | sed -n "$InitEnvBmcipLine,$InitEnvBmcipLine p")
            bmcip=${InitEnvBmcipCont#*'bmcip='} 
            #初始化bmcusr
            InitEnvBmcusrCont=$(cat $CONF_DIR/EnvConfExt | sed -n "$InitEnvBmcusrLine,$InitEnvBmcusrLine p")
            bmcusr=${InitEnvBmcusrCont#*'bmcusr='} 
            #初始化bmcpw
            InitEnvBmcpwCont=$(cat $CONF_DIR/EnvConfExt | sed -n "$InitEnvBmcpwLine,$InitEnvBmcpwLine p")
            bmcpw=${InitEnvBmcpwCont#*'bmcpw='} 
            #初始化osip
            InitEnvOsipCont=$(cat $CONF_DIR/EnvConfExt | sed -n "$InitEnvOsipLine,$InitEnvOsipLine p")
            osip=${InitEnvOsipCont#*'osip='} 
            #初始化osusr
            InitEnvOsusrCont=$(cat $CONF_DIR/EnvConfExt | sed -n "$InitEnvOsusrLine,$InitEnvOsusrLine p")
            osusr=${InitEnvOsusrCont#*'osusr='} 
            #初始化ospw
            InitEnvOspwCont=$(cat $CONF_DIR/EnvConfExt | sed -n "$InitEnvOspwLine,$InitEnvOspwLine p")
            ospw=${InitEnvOspwCont#*'ospw='} 

            # 修正bmc密码
            _autobmcpw $InitEnvBmcipLine

            #打印初始化结果
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
        else #校验失败时，返回错误信息
            clear
            echo "------------------------"
            echo "Initialization faild!!"
            echo "------------------------"
        fi
    fi
}
