################################################################
###
### Modified variable
###
### Parameter: $1 = variable name; $2 = new value
### 
################################################################


#included files
source $LIB_DIR/_CheckIpValid.sh
source $LIB_DIR/_PrintErrorInfo.sh
source $LIB_DIR/_ReorderVar.sh

function varset()
{
    if [ $# -eq 2 ]
    then
        local ValidFlag=0
        local var1=$1
        local var2=$2
        case $1 in
            "bmcip")
                _CheckIpValid $var2
                var2=$NewIp
                ValidFlag=1
                ;;
            "bmcusr")
                ValidFlag=1
                ;;
            "bmcpw")
                ValidFlag=1
                ;;
            "osip")
                _CheckIpValid $var2
                var2=$NewIp
                ValidFlag=1
                ;;
            "osusr")
                ValidFlag=1
                ;;
            "ospw")
                ValidFlag=1
                ;;  
            *) 
                ;;     
        esac

        #根据校验结果，选择是否执行操作
        if [ $ValidFlag -eq 0 ]  #无效
        then
            _PrintErrorInfo 
        else
            export $var1=$var2

            #组装Tag
            ExportGroupTag="#$globalGroupNum#:$var1"
            
            #找到line
            ExportFindLineNumExt=$(echo $(sed -n "/$ExportGroupTag/=" $CONF_DIR/EnvConfExt) | awk -F' ' '{print $1}')
            ExportReplaceLineContextExt=$(cat $CONF_DIR/EnvConfExt | sed -n "$ExportFindLineNumExt,$ExportFindLineNumExt p")

            #更新配置文件
            ExportContext="#$globalGroupNum#:$var1=$var2"
            sed -in "$ExportFindLineNumExt c "$ExportContext"" $CONF_DIR/EnvConfExt

            #回显修改值
            cat $CONF_DIR/EnvConfExt | grep $ExportGroupTag

			#重排配置文件
			_ReorderVar
        fi

    else
        _PrintErrorInfo
    fi 
}


################################################################
###
### 补全指令
### 
################################################################


_varset()
{  
	local cur prev 
	COMPREPLY=()

	cur="${COMP_WORDS[COMP_CWORD]}"
	prev="${COMP_WORDS[COMP_CWORD-1]}"

	if [ "$prev" = 'varset' ]
	then
		COMPREPLY=( $(compgen -W "bmcip bmcpw bmcusr osip ospw osusr" $cur ))
	fi

	return 0
}

complete -F _varset varset