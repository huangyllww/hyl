################################################################
###
### Description: Print function list
###
### Parameter: none
### 
################################################################


#included files

function 0help()
{
	if [ $# -eq 0 -o $# -gt 1 ]
    then
        father_file=$FUNCTION_DIR/
        for file in ${father_file}*
        do
            echo -e '  Function: '"\e[1;33m$(basename $file .sh)\e[0m"
        done
    else
        case $1 in
            "-"[vV])
            echo " extool "$(cat $BASE_DIR/ReleaseNote.txt | head -n 5 | grep -i version)
            ;;

            *)
            FuncList=($(find $FUNCTION_DIR/ -iname "*$1*.sh" -printf '%f\n' | awk -F'.sh' '{print $1}'))
            MayFuncNum=$[${#FuncList[@]}]
            
            if [[ $MayFuncNum -eq 1 ]]
            then
                cat $FUNCTION_DIR/${FuncList[1]}.sh | head -n 10 | grep -i "###" 
            else
                while [ $MayFuncNum -gt 0 ]
                do
                    echo -e '  Function: '"\e[1;33m$FuncList[$[${#FuncList[@]}-$MayFuncNum+1]]\e[0m"
                    MayFuncNum=$[$MayFuncNum-1]
                done                               
            fi
            ;;
        esac
    fi
}
