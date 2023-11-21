################################################################
###
### os log in
###
### Parameter: none
### 
################################################################


#included files

function oslogin()
{
	expect -c "
		set timeout 30 ;
		spawn ssh $osusr@$osip

		expect {
			"yes/no" { send \"yes\r\"; exp_continue }
			"password" { send $ospw\n }		
		}
		expect "#"  
		send PS1=\"\\\\
		send \"\\e\[1;33m\$\"
		send \"PS1\\\\e\[\"
		send 0m\"\r	
		interact
	"
}

