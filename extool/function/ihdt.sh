################################################################
###
### Open ihdt
###
### Parameter: none
### 
################################################################


#included files

function ihdt()
{
    echo "----------------"	
	echo -e "BMC ip :$bmcip is unlock"
    echo "----------------"	
	ipmitool -I lanplus -U $bmcusr -P $bmcpw -H $bmcip raw 0x32 0x43 0 1 
}