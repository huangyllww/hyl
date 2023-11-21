################################################################
###
### SSH login bmc sysadmin
###
### Parameter: none
### 
################################################################


#included files

function bmclogin()
{
    
    expect -c "
        set timeout 30;
        spawn ssh sysadmin@$bmcip;
        expect {
            "yes/no" { send \"yes\r\"; exp_continue }
            "password" { send \"superuser\r\" }  
        }
		expect "#"  
		send PS1=\"\\\\
		send \"\\e\[1;33m\$\"
		send \"PS1\\\\e\[\"
		send 0m\"\r	
        interact
    "
}