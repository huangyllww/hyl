################################################################
### 
### dump current environment
###
### Parameter: none
### 
################################################################


#included files

function _DumpCurVar()
{
    if [ $# -eq 0 ]
    then
        echo -e "\e[1;30m------------------------\e[0m" 
		echo -e "\e[1;33mBMC IP : \e[0m\e[1;33m$bmcip\t\e[0m"
        if [[ "$bmcusr" == "unknown" ]]
        then 
            echo -e "\e[1;30mBMC USR: \e[0m\e[1;30m$bmcusr\t\e[0m"
            echo -e "\e[1;30mBMC PW : \e[0m\e[1;30m$bmcpw\e[0m"
        else
            echo -e "\e[1;30mBMC USR: \e[0m\e[1;33m$bmcusr\t\e[0m"
            echo -e "\e[1;30mBMC PW : \e[0m\e[1;33m$bmcpw\e[0m"
        fi
        echo -e "\e[1;36m OS IP : \e[0m\e[1;36m$osip\t\e[0m"
        echo -e "\e[1;30m OS USR: \e[0m\e[1;36m$osusr\t\e[0m"
        echo -e "\e[1;30m OS PW : \e[0m\e[1;36m$ospw\e[0m"
        echo -e "\e[1;30m------------------------\e[0m"  
    fi 
}

function dump()
{
    _DumpCurVar
}