################################################################
### 
### Reorder the variables in the configuration file
### 
### Parameter: none
### 
################################################################


#included files

function _ReorderVar()
{
    local RealignBmcipArray=()
    local NumOfBmcipToSort=0

    #整理配置文件内容格式
    #找#:bmcip= ，没有则从第一行开始
    #首先筛选出全部bmcip所在行数，并赋值给变量
    RealignBmcipArray=($(awk '/#:bmcip/{print NR}' $CONF_DIR/EnvConfExt))

    #计算数组成员个数
    NumOfBmcipToSort=${#RealignBmcipArray[@]}

    #校验 globalGroupNum 是否合法
    globalGroupNumTmp=$(cat $CONF_DIR/EnvConfExt | head -n 1 | awk -F'globalGroupNum=' '{print $2}')
    if [[ $globalGroupNumTmp -gt $NumOfBmcipToSort ]]
    then
        globalGroupNum=1
        sed -rin "1s/(globalGroupNum=.*)/globalGroupNum=1/g" $CONF_DIR/EnvConfExt
    fi

    #计算数组成员个数,遍历每个成员
    while [ $NumOfBmcipToSort -gt 0 ]
    do
        RealignBmcipFirstLineNumber=${RealignBmcipArray[$NumOfBmcipToSort]}
        RealignFirstLineContext=$(sed -n "$RealignBmcipFirstLineNumber"p $CONF_DIR/EnvConfExt)

        #替换本组的全部组标识符
        sed -rin ""$RealignBmcipFirstLineNumber,$[$RealignBmcipFirstLineNumber+5]"s/(#.*#:)/#$NumOfBmcipToSort#:/g" $CONF_DIR/EnvConfExt

        #组数+1
        NumOfBmcipToSort=$[$NumOfBmcipToSort-1]

    done
    #重排结束
}
