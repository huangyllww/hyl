################################################################
###
### Environmental management
###
### Parameter: $1 = subfunction; $2 = input variable
### 
################################################################


#included files
source $LIB_DIR/_CheckIpValid.sh
source $LIB_DIR/_InitAdditionVar.sh
source $LIB_DIR/_PrintErrorInfo.sh
source $LIB_DIR/_ReorderVar.sh
source $LIB_DIR/_DumpCurVar.sh
source $LIB_DIR/_ScanIpList.sh

function iplist()
{
     
    if [ $# -eq 0 ]
    then
        _ScanIpList 'bmcip'
    else
        #新增功能： add, delete, os
        case $1 in
            "addip")
                #AddBmcipValidFlag=1
                if [ $# -eq 1 ]
                then
                    echo "------------------------"
                    echo "Input: iplist add <bmcip> "
                    echo "------------------------"
                elif [ $# -eq 2 ]
                then
                    _CheckIpValid $2
                    bmcip=$NewIp

                    #检查IP是否已存在
                    IsIpExist=$(cat $CONF_DIR/EnvConfExt | grep "#:bmcip=" | grep -i "$NewIp")
                    if [[ -n "$IsIpExist" ]]
                    then  #配置列表里有该IP, 无需添加
                        echo "\e[1;30m$bmcip\e[0m \e[1;30m已存在\e[0m"
                        SwitchEnvironment $bmcip
                        return 0
                    else  #配置列表里没有该IP, 需要添加
                        _InitAdditionVar
                    fi
                    
                else #参数数量不是1或2
                    _PrintErrorInfo
                fi
                ;;
            "delete")
                #删除某一组数据

                if [ $# -eq 2 ]
                then
                    #找到这一组的bmcip，并询问是否删除
                    if [ $2 -ge 0 -a $2 -le 255 ]
                    then
                        DeleteGroupIdStr="#$2#:"
                        DeleteGroupBmcipLine=$(echo $(sed -n "/$DeleteGroupIdStr/=" $CONF_DIR/EnvConfExt ) | awk -F' ' '{print $1}')
                        DeleteGroupBmcipLineContext=$(cat $CONF_DIR/EnvConfExt  | sed -n "$DeleteGroupBmcipLine,$DeleteGroupBmcipLine p")  
                        DeleteBmcip=${DeleteGroupBmcipLineContext#*'bmcip='} 

                        if [ -n "$DeleteGroupBmcipLine" ]
                        then
                            #read -r -p "#$2# Host:$DeleteBmcip while be deleted, are you sure? [Y/n] " input
                            echo -n "#$2# Host:$DeleteBmcip while be deleted, are you sure? [Y/n]"
                            #read "input?#$2# Host:$DeleteBmcip while be deleted, are you sure? [Y/n]?"
                            read REPLY
                            case $REPLY in
                                [Yy]) 
                                    #执行删除动作
                                    DeleteGroupBmcipLine=$[6+$DeleteGroupBmcipLine]

                                    sed -i "$DeleteGroupBmcipLine d" $CONF_DIR/EnvConfExt 
                                    sed -i "/$DeleteGroupIdStr/d" $CONF_DIR/EnvConfExt 
                                    echo "#$2# Host:$DeleteBmcip is deleted!"

                                    #整理配置文件格式
                                    _ReorderVar
                                    ;; 

                                [Nn]) 
                                    echo 'No' 
                                    ;;

                                *)
                                    echo "Invalid input..."
                                    exit 1
                                    ;;
                            esac
                        else
                            echo "------------------------" 
                            echo "no such group !!"
                            echo "------------------------" 
                        fi
                    fi
                elif [ $# -eq 1 ]
                then
                    echo "------------------------" 
                    cat $CONF_DIR/EnvConfExt  | grep -i bmcip
                    echo "------------------------" 
                    echo "Input: iplist delete <Group number>"
                    echo "------------------------"  
                else
                    _PrintErrorInfo
                fi
                ;;

            "oslist")
                _ScanIpList 'osip'
                ;;

            *)
                _PrintErrorInfo
                ;;
        esac
    fi 
}


################################################################
###
### 补全指令
### 
################################################################

_iplist()
{  
	local cur prev 
	COMPREPLY=()

	cur="${COMP_WORDS[COMP_CWORD]}"
	prev="${COMP_WORDS[COMP_CWORD-1]}"

	if [ "$prev" = 'iplist' ]
	then
		# COMPREPLY=( $(compgen -W "addip delete oslist scan" $cur ))
        COMPREPLY=( $(compgen -W "addip delete oslist" $cur ))
	fi
	return 0
}

complete -F _iplist iplist
