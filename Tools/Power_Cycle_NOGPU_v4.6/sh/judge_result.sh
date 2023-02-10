#!/bin/bash
Cur_Dir=$(cd "$(dirname "$0")";pwd)
product_name=`dmidecode -t system|grep -i "Product Name"|awk -F ":" '{print $2}'|sed 's/ //g'|head -1`
if [[ $product_name == "" ]];then
   product_name="SUT"
fi
Cur_Dir=$(cd "$(dirname "$0")";pwd)
ShAd="/var/log/$product_name/sh"
if [[ -s $ShAd/message_mce_sel_dc.txt ]];then
    rm -rf $ShAd/message_mce_sel_dc.txt
fi
list=(error fault fail failed failure warning)
show_fail_message()
{       
        tput bold
        TEXT=$@
        #echo -ne "\033[5;31m$TEXT\033[0m"
        echo -ne "\033[31m$@\033[0m"
        echo
}
show_pass_message()
{       
        tput bold
        TEXT=$@
        #echo -ne "\033[5;31m$TEXT\033[0m"
        echo -ne "\033[32m$@\033[0m"
        echo
}
for i in ${list[@]};
do  
    #echo "$i info"
    cat $ShAd/message_mce_sel.log |grep -i -w "$i" >> $ShAd/message_mce_sel_dc.txt
done
num=`cat $ShAd/message_mce_sel_dc.txt|wc -l`
if [[ "$num" -gt 0 ]];then
    show_fail_message "                                              "
    show_fail_message "                                              "
    show_fail_message "      ######       #       #####    #         "
    show_fail_message "      #           # #        #      #         "
    show_fail_message "      ######     # # #       #      #         "
    show_fail_message "      #         #     #      #      #         "
    show_fail_message "      #        #       #   #####    #####     "
	show_fail_message "                                              "
    show_fail_message "                                              "

    echo "                                              " >> $ShAd/message_mce_sel_dc.txt
    echo "                                              " >> $ShAd/message_mce_sel_dc.txt
    echo "      ######       #       #####    #         " >> $ShAd/message_mce_sel_dc.txt
    echo "      #           # #        #      #         " >> $ShAd/message_mce_sel_dc.txt
    echo "      ######     # # #       #      #         " >> $ShAd/message_mce_sel_dc.txt
    echo "      #         #     #      #      #         " >> $ShAd/message_mce_sel_dc.txt
    echo "      #        #       #   #####    #####     " >> $ShAd/message_mce_sel_dc.txt
	echo "                                              " >> $ShAd/message_mce_sel_dc.txt
    echo "                                              " >> $ShAd/message_mce_sel_dc.txt
    cat $ShAd/message_mce_sel_dc.txt
else
	show_fail_message "                                              "
    show_fail_message "                                              "
    show_pass_message "      ######       #       #####    #####     "
    show_pass_message "      #    #      # #      #        #         "
    show_pass_message "      ######     # # #     #####    #####     "
    show_pass_message "      #         #     #        #        #     "
    show_pass_message "      #        #       #   #####    #####     "
	show_fail_message "                                              "
    show_fail_message "                                              "
fi
