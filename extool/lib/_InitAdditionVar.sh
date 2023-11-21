################################################################
### 
### Add a new environment
### 
### Parameter: none
### 
################################################################

#included files
source $LIB_DIR/_DumpCurVar.sh
source $LIB_DIR/_ReorderVar.sh
source $FUNCTION_DIR/sw.sh

function _InitAdditionVar()
{
    bmcusr=admin
    bmcpw=admin
    osip=
    osusr=root
    ospw=Suma@123
    
    echo "###:bmcip=$bmcip" >> $CONF_DIR/EnvConfExt
    echo "###:bmcusr=$bmcusr" >> $CONF_DIR/EnvConfExt
    echo "###:bmcpw=$bmcpw" >> $CONF_DIR/EnvConfExt
    echo "###:osip=$osip" >> $CONF_DIR/EnvConfExt
    echo "###:osusr=$osusr" >> $CONF_DIR/EnvConfExt
    echo "###:ospw=$ospw" >> $CONF_DIR/EnvConfExt
    echo '' >> $CONF_DIR/EnvConfExt

    #整理配置文件内容格式
    _ReorderVar
    SwitchEnvironment $(echo $bmcip | awk -F'.' '{ print $4}')
}
