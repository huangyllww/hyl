################################################################
###
### Print function list
###
### Parameter: none
### 
################################################################


#included files

function biosfwupdate()
{
    
    dumpstring='Host IP:'$bmcip"\n"
    ##Set Debug Mode
    AccordedTime=$(date "+%Y-%m-%d-%H:%M:%S")
    dumpstring=$dumpstring$*
    dumpstring=$dumpstring" ("$AccordedTime")"
    echo -e $dumpstring
    echo "----------------"
    chassis power off 
	sleep 10

	tmp_dir=`pwd`
	cd $TOOL_DIR
    rm -rf sistlogs/*.* > /dev/null 2>&1
	./sistool update bios -N -H $bmcip -U $bmcusr -P $bmcpw -I $BASE_DIR/FWupdate/$1*.ROM

    cat sistlogs/*.* |grep -i Result
    rm -rf sistlogs/*.* > /dev/null 2>&1
	cd $tmp_dir
}

################################################################
###
### 补全指令
### 
################################################################
0biosfwupdate()
{ 
	biosfwupdate $1
	chassis power on
}

_biosfwupdate()
{  
	local cur prev 
	COMPREPLY=()

	cur="${COMP_WORDS[COMP_CWORD]}"
	prev="${COMP_WORDS[COMP_CWORD-1]}"
	if [ "$prev" = 'biosfwupdate' -o "$prev" = '0biosfwupdate' ]
	then
		COMPREPLY=( $(compgen -W "$(basename -s .ROM $BASE_DIR/FWupdate/*.ROM)" $cur ))
	fi
	return 0
}

complete -F _biosfwupdate biosfwupdate
complete -F _biosfwupdate 0biosfwupdate