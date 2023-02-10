#!/bin/bash
Cur_Dir=$(cd "$(dirname "$0")";pwd)

phy_threshold=100
Thresholdtemp=55

declare -a dev_id
dev_id=(\
#ixgbe
#
82598                      0x10B6 \
82598_BX                   0x1508 \
82598AF_DUAL_PORT          0x10C6 \
82598AF_SINGLE_PORT        0x10C7 \
82598AT                    0x10C8 \
82598AT2                   0x150B \
82598EB_SFP_LOM            0x10DB \
82598EB_CX4                0x10DD \
82598_CX4_DUAL_PORT        0x10EC \
82598_DA_DUAL_PORT         0x10F1 \
82598_SR_DUAL_PORT_EM      0x10E1 \
82598EB_XF_LR              0x10F4 \
82599_KX4                  0x10F7 \
82599_KX4_MEZZ             0x1514 \
82599_KR                   0x1517 \
82599_COMBO_BACKPLANE      0x10F8 \
82599_CX4                  0x10F9 \
82599_SFP                  0x10FB \
82599_BACKPLANE_FCOE       0x152A \
82599_SFP_FCOE             0x1529 \
82599_SFP_EM               0x1507 \
82599_XAUI_LOM             0x10FC \
82599_T3_LOM               0x151C \
X540                       0x1512 \
X540T                      0x1528 \
#
#ixgbevf
82599_VF                   0x10ED \
X540_VF                    0x1515 \
#
#igb
82576                      0x10C9 \
82576_FIBER                0x10E6 \
82576_SERDES               0x10E7 \
82576_QUAD_COPPER          0x10E8 \
82576_QUAD_COPPER_ET2      0x1526 \
82576_NS                   0x150A \
82576_NS_SERDES            0x1518 \
82576_SERDES_QUAD          0x150D \
82575EB_COPPER             0x10A7 \
82575EB_FIBER_SERDES       0x10A9 \
82575GB_QUAD_COPPER        0x10D6 \
82580_COPPER               0x150E \
82580_FIBER                0x150F \
82580_SERDES               0x1510 \
82580_SGMII                0x1511 \
82580_COPPER_DUAL          0x1516 \
82580_QUAD_FIBER           0x1527 \
I350_COPPER                0x1521 \
I350_FIBER                 0x1522 \
I350_SERDES                0x1523 \
I350_SGMII                 0x1524 \
I350_DA4                   0x1546 \
DH89XXCC_SGMII             0x0438 \
DH89XXCC_SERDES            0x043A \
DH89XXCC_BACKPLANE         0x043C \
DH89XXCC_SFP               0x0440 \
#
#igbvf
82576_VF                   0x10CA \
I350_VF                    0x1520 \
#
#e1000e
82571EB_COPPER             0x105E \
82571EB_FIBER              0x105F \
82571EB_SERDES             0x1060 \
82571EB_SERDES_DUAL        0x10D9 \
82571EB_SERDES_QUAD        0x10DA \
82571EB_QUAD_COPPER        0x10A4 \
82571PT_QUAD_COPPER        0x10D5 \
82571EB_QUAD_FIBER         0x10A5 \
82571EB_QUAD_COPPER_LP     0x10BC \
82572EI_COPPER             0x107D \
82572EI_FIBER              0x107E \
82572EI_SERDES             0x107F \
82572EI                    0x10B9 \
82573E                     0x108B \
82573E_IAMT                0x108C \
82573L                     0x109A \
82574L                     0x10D3 \
82574LA                    0x10F6 \
82583V                     0x150C \
80003ES2LAN_COPPER_DPT     0x1096 \
80003ES2LAN_SERDES_DPT     0x1098 \
80003ES2LAN_COPPER_SPT     0x10BA \
80003ES2LAN_SERDES_SPT     0x10BB \
ICH8_82567V_3              0x1501 \
ICH8_IGP_M_AMT             0x1049 \
ICH8_IGP_AMT               0x104A \
ICH8_IGP_C                 0x104B \
ICH8_IFE                   0x104C \
ICH8_IFE_GT                0x10C4 \
ICH8_IFE_G                 0x10C5 \
ICH8_IGP_M                 0x104D \
ICH9_IGP_M                 0x10BF \
ICH9_IGP_M_AMT             0x10F5 \
ICH9_IGP_M_V               0x10CB \
ICH9_IGP_AMT               0x10BD \
ICH9_BM                    0x10E5 \
ICH9_IGP_C                 0x294C \
ICH9_IFE                   0x10C0 \
ICH9_IFE_GT                0x10C3 \
ICH9_IFE_G                 0x10C2 \
ICH10_R_BM_LM              0x10CC \
ICH10_R_BM_LF              0x10CD \
ICH10_R_BM_V               0x10CE \
ICH10_D_BM_LM              0x10DE \
ICH10_D_BM_LF              0x10DF \
ICH10_D_BM_V               0x1525 \
PCH_M_HV_LM                0x10EA \
PCH_M_HV_LC                0x10EB \
PCH_D_HV_DM                0x10EF \
#
#e1000
82542                      0x1000 \
82543GC_FIBER              0x1001 \
82543GC_COPPER             0x1004 \
82544EI_COPPER             0x1008 \
82544EI_FIBER              0x1009 \
82544GC_COPPER             0x100C \
82544GC_LOM                0x100D \
82540EM                    0x100E \
82540EM_LOM                0x1015 \
82540EP_LOM                0x1016 \
82540EP                    0x1017 \
82540EP_LP                 0x101E \
82545EM_COPPER             0x100F \
82545EM_FIBER              0x1011 \
82545GM_COPPER             0x1026 \
82545GM_FIBER              0x1027 \
82545GM_SERDES             0x1028 \
82546EB_COPPER             0x1010 \
82546EB_FIBER              0x1012 \
82546EB_QUAD_COPPER        0x101D \
82546GB_COPPER             0x1079 \
82546GB_FIBER              0x107A \
82546GB_SERDES             0x107B \
82546GB_PCIE               0x108A \
82546GB_QUAD_COPPER        0x1099 \
82546GB_QUAD_COPPER_KSP3   0x10B5 \
82541EI                    0x1013 \
82541EI_MOBILE             0x1018 \
82541ER_LOM                0x1014 \
82541ER                    0x1078 \
82541GI                    0x1076 \
82541GI_LF                 0x107C \
82541GI_MOBILE             0x1077 \
82547EI                    0x1019 \
82547EI_MOBILE             0x101A \
82547GI                    0x1075 \
        )

NIC=0
LOM=1
nic_name="UNKOWN"
OS="UNKOWN"
declare -A pcie_addr
show_produce_message()
{
	local i
	tput bold
	TEXT=$1
	length_text=${#TEXT}
	let length_title=80-length_text	
	let half=length_title/2
	local str=""
	for((i=0;i<$half;i++))
	do
		str="$str-"
	done
	TEXT="$str"$TEXT"$str"	
	echo -ne "\033[32m$TEXT\033[0m"
	echo
}
show_fail_message()
{       
        tput bold
        TEXT=$@
        #echo -ne "\033[5;31m$TEXT\033[0m"
        echo -ne "\033[31m$@\033[0m"
        echo
}
show_item()
{
	item_key="$1"
	item_value="$2"
	
	printf "%-25s\t\t\t\t%s\n" "$item_key" "$item_value" 
}
show_title()
{
    _TEXT=$@
    tput bold
    echo -ne "\033[33m"["$_TEXT"]"\033[0m"
    echo
}
machine_info_check_tool_ver()
{
	show_produce_message "Machine Informantion Checking tool version"

	show_item "MachineCheck Infomation" "v1.1"	
	show_title "Tool Version"
	show_item "sdparm"  "1.10 20160222"
	show_item "nvme"    "nvme version 0.9"
	show_item "smartctl" "`smartctl -V | sed -n '1p' | awk '{print $1,$2}'` "
	show_item "StorCli SAS Utility" "`storcli -v |grep -i 'ver' | sed 's/.*\(Ver [0-9]*.[0-9]*.[0-9]*\).*/\1/'`" 
	show_item "LSI Corporation SAS2 IR"  "`$Cur_Dir/sas2ircu -v | grep -i version|sed 's/\(Version.*\)(.*)/\1/'`"
	show_item "LSI Corporation SAS3 IR"  "`$Cur_Dir/sas3ircu -v | grep -i version|sed 's/\(Version.*\)(.*)/\1/'`"
	show_item "IPMITOOL"  "`ipmitool -V`"

	show_produce_message "Machine Informantion Checking tool version"
}
machineSummary()
{
        show_produce_message "Machine Summary Message"
        local i

        #cpu message
        cpu_model_name=`lscpu  | grep -i 'model name' | uniq |sed 's/.*:[[:space:]]*\(.*\)/\1/'`
        cpu_nums=`lscpu | grep -i ^socket | awk -F':' '{print $NF}' | sed 's/[[:space:]]//g'`
        #mem message
        mem_manufacturer=`dmidecode -t memory | grep -i 'manufacturer' | grep -E -i -v 'unknow|no' |uniq | sed 's/.*:[[:blank:]]\(.*\)/\1/'`
        mem_nums=`dmidecode -t memory | grep -B4 'Locator' | grep -v 'Bank' | grep -i size | grep -v 'No' | wc -l`
        #pcie message
        #pcie addr
        local i
        declare -A array_ethernet
        local pci_nums=0
        show_item "cpu model name:" "$cpu_model_name"
        show_item "cpu numbers:" "$cpu_nums"
        show_item "memory manufacturer:" "$mem_manufacturer"
        show_item "memory numbers:" "$mem_nums"
        if lspci|grep -i "Ethernet">& /dev/null; then
                #temp=($(lspci|grep -i "Ethernet" | awk -F'.' '{print $1}'))
                #for((i=0;i<${#temp[@]};i++))
                #do
                #        ((array_ethernet["${temp[$i]}"]++))
                #done
                ethernetnums=`lspci | grep -i 'ethernet' | sed 's/.*Ethernet controller:\(.*\)(.*/\1/' | uniq | wc -l`
        #        show_item "Ethernet controller Numbers:" "${#array_ethernet[@]}"
        else
        #        show_item "Ethernet controller Numbers:" "0"
                ethernetnums=0
        fi
        let pci_nums=pci_nums+ethernetnums
        if lspci|grep -i -E "Network Controller|Fabric controller" >& /dev/null; then
                declare -A array_ib
                temp=($(lspci | grep -i -E "Network Controller|Fabric controller"| awk -F'.' '{print $1}'))
                for((i=0;i<${#temp[@]};i++))
                do
                        ((array_ib["${temp[$i]}"]++))
                done
                ib_nums=${#array_ib[@]}
        #        show_item "IB CARD Numbers:" "${#array_ib[@]}"
        else
        #        show_item "IB CARD Numbers:" "0"
                ib_nums=0
        fi
        let pci_nums=pci_nums+ib_nums
        if lspci|grep -i -E "lsi|Fibre" >& /dev/null; then
                local raid_flag=1
                declare -A array_hba_raid
                temp=($(lspci|grep -i -E "lsi|Fibre"| awk -F'.' '{print $1}'))
                for((i=0;i<${#temp[@]};i++))
                do
                        ((array_hba_raid["${temp[$i]}"]++))
                done
        #        show_item "RAID/HBA CARD Numbers:" "${#array_hba_raid[@]}"
                raid_hba_nums=${#array_hba_raid[@]}
        else
        #        show_item "RAID/HBA CARD Numbers:" "0"
                raid_hba_nums=0
        fi
        let pci_nums=pci_nums+raid_hba_nums
        if lspci|grep -i "3D controller" >& /dev/null; then
        #        show_item "GPU Numbers:" `lspci|grep -i "3D controller"| wc -l`
                gpu_nums=`lspci|grep -i "3D controller"| wc -l`
        else
        #        show_item "GPU Numbers:" "0"
                gpu_nums=0
        fi
        let pci_nums=pci_nums+gpu_nums
	if lspci | grep -E "Bridge: PLX" >& /dev/null; then
                declare -A array_vs
                temp=($(lspci | grep -E "Bridge: PLX"| awk -F'.' '{print $1}'))
                for((i=0;i<${#temp[@]};i++))
                do
                        ((array_vs["${temp[$i]}"]++))
                done
                vs_nums=${#array_vs[@]}
        else
                vs_nums=0
        fi
        let pci_nums=pci_nums+vs_nums
        if lspci | grep -E -i "baba" >& /dev/null; then
                declare -A array_vs
                temp=($(lspci | grep -E -i "baba"| awk -F'.' '{print $1}'))
                for((i=0;i<${#temp[@]};i++))
                do
                        ((array_vs["${temp[$i]}"]++))
                done
                fpga_nums=${#array_vs[@]}
        else
                fpga_nums=0
        fi
        let pci_nums=pci_nums+fpga_nums

        show_item "pcie devices:" "$pci_nums"
        show_item "Ethernet controller Numbers:" "$ethernetnums"
        show_item "IB CARD Numbers:" "$ib_nums"
        show_item "RAID/HBA CARD Numbers:" "$raid_hba_nums"
	show_item "Vedio Speed Numbers:" "$vs_nums"
        show_item "FPGA Numbers:" "$fpga_nums"

        if  nvme list | sed -n '/\/dev/p' | awk '{print $1}' | sed 's/..$//'| sed '/^$/d'| wc -l >& /dev/null ; then
                show_item "NVME Numbers:" `nvme list | sed -n '/\/dev/p' | awk '{print $1}' | sed 's/..$//'| sed '/^$/d' | wc -l`
        fi
        if [ ${raid_flag:-0} -eq 0 ]; then
                sys_hdd_backup=($(fdisk -l 2>/dev/null|grep dev|grep Disk|grep -v "mapper\|nvme"|sort | sed 's/.*\(\/dev\/.*\):.*/\1/' | sed 's/\/dev\/\(.*\)/\1/'))
                show_item "HDD Numbers:" `fdisk -l 2>/dev/null|grep dev|grep Disk|grep -v "mapper\|nvme"|sort | wc -l`
        else
                sum_hdds=0
                #sys_hdd=($(lsblk | grep -i -E 'sd[a-z]*' | awk '{print $1}'|grep -v '[1-9]'))
                sys_hdd=($(fdisk -l 2>/dev/null|grep dev|grep Disk|grep -v "mapper\|nvme"|sort | sed 's/.*\(\/dev\/.*\):.*/\1/' | sed 's/\/dev\/\(.*\)/\1/'))
                rm -f *.txt
                if lspci | grep -i 'raid' >& /dev/null ; then
                        raid_card_link=`storcli -pdlist --aALL|grep -i "PD type"|wc -l`
                        raid_hdds_sn=($(storcli /call/eall/sall show all | grep -i 'sn =' | sed 's/.*=[[:blank:]]\([0-9A-Z]*\)/\1/'))
                        if [ $raid_card_link -ne ${#raid_hdds_sn[@]} ]; then
                                show_fail_message "Some the raid card link hdd status is not good and lost the hdd sn,please check"
                        fi
                        raid_hdd_sn_max=${#raid_hdds_sn[0]}
                        for((i=0;i<${#raid_hdds_sn[@]};i++))
                        do
                                if [ $raid_hdd_sn_max -lt ${#raid_hdds_sn[$i]} ]; then
                                        raid_hdd_sn_max=${#raid_hdds_sn[$i]}
                                fi
                        done
                        len=${#sys_hdd[@]}
                        for((i=0;i<$len;i++))
                        do
                                sys_sn=`smartctl -i /dev/${sys_hdd[$i]} | grep -i 'serial number' | sed 's/.*:[[:space:]]*\([0-9A-Z]*\)/\1/'`
                                sys_vendor=`smartctl -i /dev/${sys_hdd[$i]}  | grep -i 'vendor' | awk '{print $NF}'`
                                #if [[ ${#sys_sn} -eq 0 ]] || [[ ${#sys_sn} -gt $raid_hdd_sn_max ]] || [[ "$sys_vendor" =~ "AVAGO" ]] || [[ "$sys_vendor" =~ "LSI" ]]; then
                                #if [[ ${#sys_sn} -eq 0 ]] || [[ "$sys_vendor" =~ "AVAGO" ]] || [[ "$sys_vendor" =~ "LSI" ]]; then
                                if [[ "$sys_vendor" =~ "AVAGO" ]] || [[ "$sys_vendor" =~ "LSI" ]]; then
                                        unset sys_hdd[$i]
                                        continue
                                fi
                        done

                        echo ${raid_hdds_sn[@]} | sed 's/ /\n/g' >raid_hdds_sn.txt
                                new_sys_hdd=($(echo ${sys_hdd[@]}))
                                outer_total=${#new_sys_hdd[*]}
                                inner_total=${#raid_hdds_sn[*]}
                                for((j=0;j<$outer_total;j++))
                                do
                                        sys_sn_temp=`smartctl -i /dev/${new_sys_hdd[$j]} | grep -i 'serial number' | sed 's/.*:[[:space:]]*\([0-9A-Z]*\)/\1/'`
                                        for((k=0;k<$inner_total;k++))
                                        do
                                                if [ "$sys_sn_temp" == "${raid_hdds_sn[$k]}" ]; then
                                                        unset new_sys_hdd[$j]                  
                                                        break
                                                fi
                                        done
                                done
                                card_load_hdd=${#raid_hdds_sn[*]}
                                hdd_nums=card_load_hdd
                                let sum_hdds=sum_hdds+hdd_nums
                                sys_hdd=($(echo ${new_sys_hdd[@]}))

                fi
                sys_hdd_backup=${sys_hdd[@]}
                if lspci | grep -i 'lsi'|grep -E '2008|2308' >& /dev/null; then
                        hba_nums=`lspci | grep -i 'lsi'|grep -E '2008|2308' | wc -l`
                        for((i=0;i<$hba_nums;i++))
                        do
                                #./sas2ircu $i DISPLAY |grep -i "serial no" | sed 's/.*:[[:blank:]]\([0-9A-Z]*\)/\1/' >> hba_hdds_sn.txt
                                $Cur_Dir/sas2ircu $i DISPLAY |grep -i "serial" | sed 's/.*:[[:blank:]]\([0-9A-Z]*\)/\1/' | uniq >> hba_hdds_sn.txt
                        done
                        hba_hdds_sn=($(cat hba_hdds_sn.txt))
                        hba_hdd_sn_max=${#hba_hdds_sn[0]}
                        for((i=0;i<${#hba_hdds_sn[@]};i++))
                        do
                                if [ $hba_hdd_sn_max -lt ${#hba_hdds_sn[$i]} ]; then
                                        hba_hdd_sn_max=${#hba_hdds_sn[$i]}
                                fi
                        done
                        len=${#sys_hdd[@]}
                        for((i=0;i<$len;i++))
                        do
                                sys_sn=`smartctl -i /dev/${sys_hdd[$i]} | grep -i 'serial number' | sed 's/.*:[[:space:]]*\([0-9A-Z]*\)/\1/'`
                                sys_vendor=`smartctl -i /dev/${sys_hdd[$i]}  | grep -i 'vendor' | awk '{print $NF}'`
                                #if [[ ${#sys_sn} -eq 0 ]] || [[ ${#sys_sn} -gt $raid_hdd_sn_max ]] || [[ "$sys_vendor" =~ "AVAGO" ]] || [[ "$sys_vendor" =~ "LSI" ]]; then
                                if [[ ${#sys_sn} -gt $raid_hdd_sn_max ]] || [[ "$sys_vendor" =~ "AVAGO" ]] || [[ "$sys_vendor" =~ "LSI" ]]; then
                                        unset sys_hdd[$i]
                                        continue
                                fi
                        done
                        sys_hdd_backup=${sys_hdd[@]}
                        cat hba_hdds_sn.txt | sed '/^$/d' > hba_hdds_sn.temp
                        array_hba_hdds_sn=($(cat hba_hdds_sn.temp))
                        new_sys_hdd=($(echo ${sys_hdd[@]}))
                        outer_total=${#new_sys_hdd[@]}
                        inner_total=${#array_hba_hdds_sn[@]}
                        for((j=0;j<$outer_total;j++))
                        do
                                sys_sn_temp=`smartctl -i /dev/${new_sys_hdd[$j]} | grep -i 'serial number' | sed 's/.*:[[:space:]]*\([0-9A-Z]*\)/\1/'`
                                for((k=0;k<$inner_total;k++))
                                do
                                        if [ "$sys_sn_temp" == "${array_hba_hdds_sn[$k]}" ]; then
                                                unset new_sys_hdd[$j]                  
                                                break
                                        fi
                                done
                        done
                        card_load_hdd=${#array_hba_hdds_sn[*]}
                        hdd_nums=card_load_hdd
                        let sum_hdds=sum_hdds+hdd_nums
                        sys_hdd=($(echo ${new_sys_hdd[@]}))
                fi
                if lspci | grep -i 'lsi'|grep -E '3008' >& /dev/null; then
                        hba_nums=`lspci | grep -i 'lsi'|grep -E '3008' | wc -l`
                        for((i=0;i<$hba_nums;i++))
                        do
                                #./sas3ircu $i DISPLAY |grep -i "serial no" | sed 's/.*:[[:blank:]]\([0-9A-Z]*\)/\1/' >> hba_hdds_sn.txt
                                $Cur_Dir/sas3ircu $i DISPLAY |grep -i "serial no" | sed 's/.*:[[:blank:]]\([0-9A-Z]*\)/\1/' | uniq >> hba_hdds_sn.txt
                        done
                        hba_hdds_sn=($(cat hba_hdds_sn.txt))
                        hba_hdd_sn_max=${#hba_hdds_sn[0]}
                        for((i=0;i<${#hba_hdds_sn[@]};i++))
                        do
                                if [ $hba_hdd_sn_max -le ${#hba_hdds_sn[$i]} ]; then
                                        hba_hdd_sn_max=${#hba_hdds_sn[$i]}
                                fi
                        done
                        len=${#sys_hdd[@]}
                        for((i=0;i<$len;i++))
                        do
                                sys_sn=`smartctl -i /dev/${sys_hdd[$i]} | grep -i 'serial number' | sed 's/.*:[[:space:]]*\([0-9A-Z]*\)/\1/'`
                                sys_vendor=`smartctl -i /dev/${sys_hdd[$i]}  | grep -i 'vendor' | awk '{print $NF}'`
                                #if [[ ${#sys_sn} -eq 0 ]] || [[ ${#sys_sn} -gt $hba_hdd_sn_max ]] || [[ "$sys_vendor" =~ "AVAGO" ]] || [[ "$sys_vendor" =~ "LSI" ]]; then
                                #if [[ ${#sys_sn} -eq 0 ]] || [[ "$sys_vendor" =~ "AVAGO" ]] || [[ "$sys_vendor" =~ "LSI" ]]; then
                                if [[ "$sys_vendor" =~ "AVAGO" ]] || [[ "$sys_vendor" =~ "LSI" ]]; then
                                        unset sys_hdd[$i]
                                        continue
                                fi
                        done
                        sys_hdd_backup=${sys_hdd[@]}

                        cat hba_hdds_sn.txt | sed '/^$/d' > hba_hdds_sn.temp
                        array_hba_hdds_sn=($(cat hba_hdds_sn.temp))
                        new_sys_hdd=($(echo ${sys_hdd[@]}))
                        outer_total=${#new_sys_hdd[@]}
                        inner_total=${#array_hba_hdds_sn[@]}
                        for((j=0;j<$outer_total;j++))
                        do
                                #sys_sn_temp=`smartctl -i /dev/${new_sys_hdd[$j]} | grep -i 'serial number' | sed 's/.*:[[:space:]]*\([0-9A-Z]*\)/\1/'`
                                sys_sn_temp=`smartctl -i /dev/${new_sys_hdd[$j]} | grep -i 'serial' | sed 's/.*:[[:space:]]*\([0-9A-Z]*\)/\1/'`
                                for((k=0;k<$inner_total;k++))
                                do
                                        if [ "$sys_sn_temp" == "${array_hba_hdds_sn[$k]}" ]; then
                                                unset new_sys_hdd[$j]                  
                                                break
                                        fi
                                done
                        done
                        hdd_nums=${#array_hba_hdds_sn[*]}
                        sys_hdd=($(echo ${new_sys_hdd[*]}))
                        let sum_hdds=sum_hdds+hdd_nums
                        sys_hdds_nums=${#sys_hdd[*]}
                        let sum_hdds=sum_hdds+sys_hdds_nums
                fi
                show_item "HDD Numbers:" $sum_hdds
        fi
        rm -f *.txt
        rm -f *.temp
 #       show_item "USB Numbers:" `lsusb | grep -E -i 'Linux Foundation|American Megatrends'|wc -l`
        show_produce_message "Machine Summary Message"
}
os_board_info()
{
	show_produce_message "OS Board SMBIOS Message"

	show_title "OS"
	os=`cat /etc/redhat-release 2>/dev/null`
	kernel=`uname -a`
	show_item "OS:" "$os"
	show_item "Kernel:" "$kernel"

	show_title "BIOS"
	bios_version=`dmidecode -t bios | grep -E "Version"| awk -F ":" '{print $2}'|sed 's/^[[:space:]]*//g'|sed 'N;s/\n/-/g'`
	release_date=`dmidecode -t bios | grep -E "Release Date"| awk -F ":" '{print $2}'|sed 's/^[[:space:]]*//g'|sed 'N;s/\n/-/g'`
	address=`dmidecode -t bios | grep -E "Address"| awk -F ":" '{print $2}'|sed 's/^[[:space:]]*//g'|sed 'N;s/\n/-/g'`
	runtime_size=`dmidecode -t bios | grep -E "Runtime Size"| awk -F ":" '{print $2}'|sed 's/^[[:space:]]*//g'|sed 'N;s/\n/-/g'`
	rom_size=`dmidecode -t bios | grep -E "ROM Size"| awk -F ":" '{print $2}'|sed 's/^[[:space:]]*//g'|sed 'N;s/\n/-/g'`
	bios_revision=`dmidecode -t bios | grep -E "BIOS Revision"| awk -F ":" '{print $2}'|sed 's/^[[:space:]]*//g'|sed 'N;s/\n/-/g'`
	show_item "BIOS Version:" "$bios_version"
	show_item "Release Date:" "$release_date"
	show_item "Address:" "$address"
	show_item "Runtime Size:" "$runtime_size"
	show_item "ROM Size:" "$rom_size"
	show_item "BIOS Revision:" "$bios_revision"
#	dmidecode -t bios | sed -n '/Characteristics:/,$p'

	show_title "BMC Version"
	bmc_version=`ipmitool mc info | grep "Firmware Revision" | awk -F ":" '{print $2}' |sed 's/^[[:space:]]*//g' 2>/dev/null`
	show_item "BMC Version:"  "$bmc_version"

	show_title "Board"
	manufacture_system=`dmidecode -t system | grep 'Manufacture'| uniq |awk -F ":" '{print $2}'|sed 's/^[[:space:]]*//g'`
	manufacture_board=`dmidecode -t baseboard | grep 'Manufacture'| uniq |awk -F ":" '{print $2}'|sed 's/^[[:space:]]*//g'`
	product_name_system=`dmidecode -t system | grep 'Product Name'| uniq |awk -F ":" '{print $2}'|sed 's/^[[:space:]]*//g'`
	product_name_board=`dmidecode -t baseboard | grep 'Product Name'| uniq |awk -F ":" '{print $2}'|sed 's/^[[:space:]]*//g'`
	system_version=`dmidecode -t system | grep 'Version'| uniq |awk -F ":" '{print $2}'|sed 's/^[[:space:]]*//g'`
	board_version=`dmidecode -t baseboard | grep 'Version'| uniq |awk -F ":" '{print $2}'|sed 's/^[[:space:]]*//g'`
	sn_system=`dmidecode -t system | grep 'Serial Number'| uniq |awk -F ":" '{print $2}'|sed 's/^[[:space:]]*//g'`
	sn_board=`dmidecode -t baseboard | grep 'Serial Number'| uniq |awk -F ":" '{print $2}'|sed 's/^[[:space:]]*//g'`
	
	show_item "Manufacture of system:" "$manufacture_system"
	show_item "Manufacture of board:" "$manufacture_board"
	show_item "Product Name of system:" "$product_name_system"
	show_item "Product Name of board:" "$product_name_board"
	show_item "Version of system:" "$system_version"
	show_item "Version of board:" "$board_version"
	show_item "Serial Number of system:" "$sn_system"
	show_item "Serial Number of board:" "$sn_board"
	service ipmi start 1>/dev/null 2>/dev/null
	
	show_produce_message "OS Board SMBIOS Message"
}
cpuinfo()
{
        show_produce_message "CPU Detail Message"
        local i
        cpu_type=`lscpu | grep -i 'Vendor ID:' | awk -F':' '{print $NF}' | sed 's/[[:space:]]//g'`
        show_title "$cpu_type"
        if [[ "$cpu_type" =~ "Intel" ]]; then
        for((i=0;i<$cpu_nums;i++))
        do
                cpu_phy_id=`cat /proc/cpuinfo | grep -i 'physical id' |  sed -n "/$i/p" | awk -F':' '{print $NF}' | sed 's/[[:space:]]//g' | uniq`
                cpu_vendor=`dmidecode -t processor | grep -A40 "CPU"$i |grep 'Version'|head -1| awk -F ":" '{print $2}'|sed 's/^[[:space:]]*//g'|awk '{print $1}'`
                cpu_model=`dmidecode -t processor | grep -A40 "CPU"$i |grep 'Version' |uniq | awk -F':' '{print $NF}' | sed 's/[[:space:]]*//'`
                cpu_cache=`cat /proc/cpuinfo | grep -i -E -B 3 "physical id.*$i" | uniq |grep -i 'cache' | uniq | awk -F':' '{print $NF}' | sed 's/[[:space:]]*//'`
                frequency=`dmidecode -t processor | grep -A40 "CPU"$i |grep 'Version' |uniq | awk '{print $NF}'`
                physical_cores=`cat /proc/cpuinfo | grep -i -E -B10 -A9 "physical id.* $i" | grep -i -E "cpu cores" | awk -F':' '{print $NF}' | uniq | sed 's/[[:space:]]//'`

                cpu_avx=`cat /proc/cpuinfo | grep "avx" | uniq `
                current_speed=`dmidecode -t processor | grep -A40 "CPU$i" | grep -i "current speed" | awk -F':' '{print $NF}' | sed 's/^.//'`
                show_item "cpu$i physical id:" "$cpu_phy_id"
                show_item "cpu$i vendor:" "$cpu_vendor"
                show_item "cpu$i model:" "$cpu_model"
                show_item "cpu$i cache:" "$cpu_cache"
                show_item "cpu$i frequency:" "$frequency"
                show_item "cpu$i physical:" "$physical_cores"
                show_item "cpu$i speed:" "$current_speed"
                show_item "cpu$i avx:" "$cpu_avx"
                printf "\n"
        done
        fi
        if [[ "$cpu_type" =~ "AMD" ]]; then
                for((i=0;i<$cpu_nums;i++))
                do
                        cpu_phy_id=`cat /proc/cpuinfo | grep -i 'physical id' |  sed -n "/$i/p" | awk -F':' '{print $NF}' | sed 's/[[:space:]]//g' | uniq`
                        cpu_model=`cat /proc/cpuinfo | grep -i -E -B10 -A9 "physical id.* $i" | grep -i -E "model name" | awk -F':' '{print $NF}' | uniq | sed 's/[[:space:]]//'`
                        cpu_cache=`cat /proc/cpuinfo | grep -i -E -B10 -A9 "physical id.* $i" | grep -i -E "cache size" | awk -F':' '{print $NF}' | uniq | sed 's/[[:space:]]//'`
                        frequency=`cat /proc/cpuinfo | grep -i -E -B10 -A9 "physical id.* $i" | grep -i -E "cpu MHz" | awk -F':' '{print $NF}' | uniq | sed 's/[[:space:]]//'`
                        physical_cores=`cat /proc/cpuinfo | grep -i -E -B10 -A9 "physical id.* $i" | grep -i -E "cpu cores" | awk -F':' '{print $NF}' | uniq | sed 's/[[:space:]]//'`

                        cpu_avx=`cat /proc/cpuinfo | grep "avx" | uniq `
                        current_speed=`dmidecode -t processor | grep -i "current speed" | head -n1 |awk -F':' '{print $NF}' | sed 's/^.//'`

                        show_item "cpu$i physical id:" "$cpu_phy_id"
                        show_item "cpu$i vendor:" "$cpu_vendor"
                        show_item "cpu$i model:" "$cpu_model"
                        show_item "cpu$i cache:" "$cpu_cache"
                        show_item "cpu$i frequency:" "$frequency"
                        show_item "cpu$i physical:" "$physical_cores"
                        show_item "cpu$i speed:" "$current_speed"
                        show_item "cpu$i avx:" "$cpu_avx"
                        printf "\n"
                done
        fi
        show_title "Numa Info:"
        numactl --show
#       cpupower monitor
        show_produce_message "CPU Detail Message"
}
upiinfo(){
   cpu_nums=`lscpu | grep -i ^socket | awk -F':' '{print $NF}' | sed 's/[[:space:]]//g'`
   realityCPUNum=`cat /proc/cpuinfo | grep -i 'physical id' | sort | uniq | wc -l`
   if [[ $realityCPUNum -ge 2 ]]; then
      if [[ $cpu_nums -ge 4 ]] && [[ $realityCPUNum -le 2 ]]; then
         return 0
      fi
   show_produce_message "UPI Message"
   lspci|grep -i "205b">tmp
   declare -A upi_array
   if [ -s tmp ];then
      linenum=`awk '{print NR}' tmp|tail -n1`
      while read line
      do
         addr=`echo $line|awk '{print $1}'`
         echo "Bus:device:func: " $addr
         lspci -s $addr -xxxvvv|grep -i ^d0|grep -v d00
         ret=`lspci -s $addr -xxxvvv|grep -i ^d0|grep -v d00|awk '{print $6}'`
         if [[ ret -eq 4 ]];then
            echo "Check offset d4, Link Speed : 9.6 GT/s"
            upi_array["$addr"]="Check offset d4, Link Speed : 9.6 GT/s"
         elif [[ ret -eq 5 ]];then
            echo "Check offset d4, Link Speed : 10.4 GT/s"
            upi_array["$addr"]="Check offset d4, Link Speed : 10.4 GT/s"
         else
            echo "Your system QPI speed is Slow mode, please check and confirm your processor configurtion"
            touch upi_speed_illegal.flag
         fi
         ret=`lspci -s $addr -xxxx|grep -i 130|awk '{for(i=1;i<=NF;i++) if ($i==1) {print "fail";break}}'`
         if [[ $ret == "fail" ]];then
            echo "All UPI Lane is '1'.Fail!"
            touch upi_speed_illegal.flag
         else
            echo "All UPI Lane is '0'.Pass!"
         fi
      done < tmp
   fi
   lspci|grep -i -E "2059" | grep -i -E "0e">tmp
   if [ -s tmp ];then
      while read line 
      do
         addr=`echo $line|awk '{print $1}'`
         dev=`echo $addr|awk -F ":" '{print $2}'|awk -F "." '{print $1}'`
         if [[ $dev != "10" ]];then
            ret=`lspci -s $addr -xxxx|grep -i "140"|awk '{print $10}'`
            ret10=`echo $((16#$ret))`
            ret2=`echo "obase=2;$ret10"|bc`
            ret2Len=$((${#ret2}-1))
            retLastBit=${ret2:ret2Len:1}
            retCpu0Flag=`echo $addr |grep -i -E "5d"`
            if [ "$retCpu0Flag" != "" ]; then
               CPUIndex=0
            else
               retCpu1Flag=`echo $addr |grep -i -E "d7"`
               if [ "$retCpu1Flag" != "" ]; then
                  CPUIndex=1
               else
                  CPUIndex="CPU"
               fi
            fi
            if [[ "$retLastBit" == "1" ]];then
                  echo "UPI Speed State is right."
            else
               echo "Your system $CPUIndex S_OP_SPEED is $retLastBit UPI is Slow mode, please check and confirm your processor configurtion"
               touch upi_speed_illegal.flag
            fi
            ret=`lspci -s $addr -xxxx|grep -i "150"|awk -F" " '{print $6}' | sed 's/[[:space:]]//g'`
            retLen=$((${#ret}-1))
            retLastBit=${ret:retLen:1}
            retFirstBit=${ret:0:1}
            if [[ $ret == "00" ]];then
               echo "S_TX_STATE_P S_RX_State is right"
            else
               if [ "$retFirstBit" != "0" ]; then
                  echo "$CPUIndex S_TX_STATE_P is error"        
               fi
               if [ "$retLastBit" != "0" ]; then
                  echo "$CPUIndex R_RX_STATE_P is error"        
               fi
               touch upi_speed_illegal.flag
	    fi
         fi
      done < tmp
   fi
   if [ ! -s tmp ];then
      show_fail_message "There is not exist UPI in this machine"
   fi
   rm -rf tmp
   show_produce_message "UPI Message"
   else
        show_fail_message "The number of cpu is lower than 2,so do not check upi speed"
   fi
}
:<<!
upiinfo(){
   cpu_nums=`lscpu | grep -i ^socket | awk -F':' '{print $NF}' | sed 's/[[:space:]]//g'`
   realityCPUNum=`cat /proc/cpuinfo | grep -i 'physical id' | sort | uniq | wc -l`
   if [[ $realityCPUNum -ge 2 ]]; then
      if [[ $cpu_nums -ge 4 ]] && [[ $realityCPUNum -le 2 ]]; then
         return 0
      fi
   show_produce_message "UPI Message"
   lspci|grep -i "205b">tmp
   declare -A upi_array
   if [ -s tmp ];then
      linenum=`awk '{print NR}' tmp|tail -n1`
      while read line
      do
         addr=`echo $line|awk '{print $1}'`
         echo "Bus:device:func: " $addr
         lspci -s $addr -xxxvvv|grep -i ^d0|grep -v d00
         ret=`lspci -s $addr -xxxvvv|grep -i ^d0|grep -v d00|awk '{print $6}'`
         if [[ ret -eq 4 ]];then
            echo "Check offset d4, Link Speed : 9.6 GT/s"
	    upi_array["$addr"]="Check offset d4, Link Speed : 9.6 GT/s"
         elif [[ ret -eq 5 ]];then
            echo "Check offset d4, Link Speed : 10.4 GT/s"
	    upi_array["$addr"]="Check offset d4, Link Speed : 10.4 GT/s"
         else
            echo "Check offset d4, Link Speed : illegal"
         fi
         ret=`lspci -s $addr -xxxx|grep -i 130|awk '{for(i=1;i<=NF;i++) if ($i==1) {print "fail";break}}'`
         if [[ $ret == "fail" ]];then
            echo "All UPI Lane is '1'.Fail!"
         else
            echo "All UPI Lane is '0'.Pass!"
         fi
      done < tmp
   fi
   lspci|grep -i "2059">tmp
   if [ -s tmp ];then
      while read line 
      do
         addr=`echo $line|awk '{print $1}'`
         dev=`echo $addr|awk -F ":" '{print $2}'|awk -F "." '{print $1}'`
         if [[ $dev != "10" ]];then
            ret=`lspci -s $addr -xxxx|grep -i "140"|awk '{print $10}'`
            if [[ $ret == "1f" ]];then
               echo "UPI Speed State is right."
            else
               echo "UPI Speed is slow,and exit now..."
            fi
            ret=`lspci -s $addr -xxxx|grep -i "150"|cut -d " " -f6-9`
            if [[ $ret == "00 00 00 00" ]];then
               echo "S_RX_State is right"
            else
               echo "S_RX_State is error"
            fi
         fi
      done < tmp
   fi
   if [ ! -s tmp ];then
       show_fail_message "There is not exist UPI in this machine"
   fi
   rm -rf tmp
   show_produce_message "UPI Message"
   else
        show_fail_message "The number of cpu is lower than 2,so do not check upi speed"
   fi
}
!
IsNull()
{

z=`dmidecode -t memory|grep "$1"|wc -l`
x=`dmidecode -t memory|grep "Memory Device"|wc -l`
if [ $z -eq 0 ]; then
 while [ $y -le $x ]
 do
 echo "N/A" >> ./$2
 y=`echo "$y + 1" | bc`
 done
 fi
}
ToG(){
if [[ "$1" =~ "kB" ]];then
      ret=`echo $1 |sed 's/K//g' |sed 's/M//g'|sed 's/B//g'|awk '{printf "%.0lf",$1/1048576}'`GB
      echo $ret
   elif [[ "$1" =~ "MB" ]];then
      ret=`echo $1 |sed 's/K//g' |sed 's/M//g'|sed 's/B//g'|awk '{printf "%.0lf",$1/1024}'`GB
      echo $ret
   else
      echo $1
   fi
}
MemoryTopo(){
memory_vendor=`dmidecode -t memory|grep "Manufacturer:"|grep -v "NO DIMM"|awk -F: '{print $2}'|head -1`
memory_sn=`dmidecode -t memory|grep "Part Number:"|grep -v "NO DIMM"|awk -F: '{print $2}'|head -1`
dmidecode -t memory|grep "Manufacturer:"|awk -F: '{print $2}'>>por.txt
odd=`awk 'NR%2' por.txt`
for str in $odd
do
   if [ " $str"x = "$memory_vendor"x ];then
      odd_flag=1
      continue
   else
      odd_flag=0
      break
   fi
done
for str in $odd
do
   if [[ $odd_flag -eq 0 ]];then
      if [ "$str"x != "$memory_vendor"x ];then
         odd_flag=2
         continue
      else
         odd_flag=0
         break
      fi
   fi
done
even=`awk '!(NR%2)' por.txt`
for str in $even
do
   if [ x" $str" = x"$memory_vendor" ];then
      even_flag=1
      continue
   else
      even_flag=0
      break
   fi
done
for str in $even
do
   if [[ $even_flag -eq 0 ]];then
      if [ "$str"x != "$memory_vendor"x ];then
         even_flag=2
         continue
      else
         even_flag=0
         break
      fi
   fi
done

if [ $odd_flag -eq 1 -a $even_flag -eq 1 ];then
   show_item "POR:" "2DPC"
elif [ $odd_flag -eq 1 -a $even_flag -eq 2 ];then
   show_item "POR:" "1DPC"
else
   show_item "POR:" "other"
fi

sed -i '/^ *$/d' por.txt
num=`awk 'END{print NR}' por.txt`
num_half=`echo "$num / 2" | bc`
up=`sed -n "1,$num_half p" por.txt`
num_half=`echo "$num_half + 1" | bc`
down=`sed -n "${num_half},$num p" por.txt`
if [ "$up"x = "$down"x ];then
   show_item "Topo:" "symmetrical"
else
   show_item "Topo:" "non-symmetrical"
fi
rm -rf por.txt
        
}
######## MemoryTopo include AEP ############
MemoryTopo_for_AEP(){
memory_vendor1=`dmidecode -t memory|grep "Manufacturer:"|grep -v "NO DIMM"|awk -F: '{print $2}'|head -1`
memory_vendor2=`dmidecode -t memory|grep "Manufacturer:"|grep -v "NO DIMM"|awk -F: '{print $2}'|sed -n 2p`
memory_sn=`dmidecode -t memory|grep "Part Number:"|grep -v "NO DIMM"|awk -F: '{print $2}'|head -1`
dmidecode -t memory|grep "Manufacturer:"|awk -F: '{print $2}'>>por.txt
odd=`awk 'NR%2' por.txt`
for str in $odd
do
   if [ " $str"x = "$memory_vendor1"x ];then
      odd_flag=1
      continue
   else
      odd_flag=0
      break
   fi
done
for str in $odd
do
   if [[ $odd_flag -eq 0 ]];then
      if [ "$str"x != "$memory_vendor1"x ];then
         odd_flag=2
         continue
      else
         odd_flag=0
         break
      fi
   fi
done
even=`awk '!(NR%2)' por.txt`
for str in $even
do
   if [ x" $str" = x"$memory_vendor2" ];then
      even_flag=1
      continue
   else
      even_flag=0
      break
   fi
done
for str in $even
do
   if [[ $even_flag -eq 0 ]];then
      if [ "$str"x != "$memory_vendor2"x ];then
         even_flag=2
         continue
      else
         even_flag=0
         break
      fi
   fi
done

if [ $odd_flag -eq 1 -a $even_flag -eq 1 ];then
   show_item "POR:" "2DPC"
elif [ $odd_flag -eq 1 -a $even_flag -eq 2 ];then
   show_item "POR:" "1DPC"
else
   show_item "POR:" "other"
fi

sed -i '/^ *$/d' por.txt
num=`awk 'END{print NR}' por.txt`
num_half=`echo "$num / 2" | bc`
up=`sed -n "1,$num_half p" por.txt`
num_half=`echo "$num_half + 1" | bc`
down=`sed -n "${num_half},$num p" por.txt`
if [ "$up"x = "$down"x ];then
   show_item "Topo:" "symmetrical"
else
   show_item "Topo:" "non-symmetrical"
fi
rm -rf por.txt
        
}
###################################
MemoryInfo(){
   local i=1
   slot=`dmidecode -t memory|grep "Memory Device"|wc -l`
   dimm=`dmidecode -t memory|grep "Size"|grep -E "GB|MB"|wc -l`
   size=`dmidecode -t memory|grep "Size"|grep -E "GB|MB"|awk -F ":" {'print $2'}|grep -i -v "Unknown" |head -1 | awk '{print $1}'`
   unit=`dmidecode -t memory|grep "Size"|grep -E "GB|MB"|awk -F ":" {'print $NF'}|head -1 | awk '{print $NF}'`
   mem_nums=`dmidecode -t memory | grep -B4 'Locator' | grep -v 'Bank' | grep -i size | grep -v 'No' | wc -l`
   let total_size=mem_nums*size
   total_size="$total_size $unit"
   total_size=`ToG "$total_size"`
   size="$size $unit"
   size=`ToG "$size"`
   freq=`dmidecode -t memory|grep "Configured Clock Speed:"|awk -F: '{print $2}'| grep -i -v "Unknown"|head -1 | sed 's/.//'`
   DDRx=`dmidecode -t memory|grep "Type"|grep -v "Detail"|grep -v "Error Correction Type"| grep -i -v "Unknown" |awk -F: '{print $2}'|head -1 | sed 's/^.//'`
   show_item "Physical Slots:" "$slot"
   show_item "Populated DIMMs:" "$dimm"
   show_item "Populated dimm_size:" "$size"
   show_item "Memory Total Capacity:" "$total_size"
   show_item "Freqency:" "$freq"
   show_item "DDRx:" "$DDRx"
   MemoryTopo
   echo "the detail of memory population :"
   echo "==============================="
   dmidecode -t memory|grep "Locator"|grep -v Bank|awk -F: '{print $2}'>./1.txt
   dmidecode -t memory|grep "Size"|awk -F: '{print $2}'> ./2.txt
   dmidecode -t memory|grep "Manufacturer:"|awk -F: '{print $2}'>./3.txt
   dmidecode -t memory|grep "Part Number:"|awk -F: '{print $2}'>./4.txt
   dmidecode -t memory|grep "Serial Number:"|awk -F: '{print $2}'>./5.txt
   dmidecode -t memory|grep "Configured Clock Speed:"|awk -F: '{print $2}'>./6.txt
   IsNull Type 7.txt
   dmidecode -t memory|grep "Type"|grep -v "Detail"|grep -v "Error Correction Type"|awk -F: '{print $2}'>>./7.txt
   printf "%-15s%-10s%-10s%-25s%-10s%-10s\n" "Locator" "Size" "Vendor" "Part_Number" "Speed" "Type"
   while [[ $i -le $slot ]]; do
        c=`sed -n "$i p" 1.txt`
        d=`sed -n "$i p" 2.txt`
        e=`sed -n "$i p" 3.txt`
        f=`sed -n "$i p" 4.txt`
        h=`sed -n "$i p" 6.txt`
        g=`sed -n "$i p" 7.txt`
        printf "%-15s%-10s%-10s%-25s%-10s%-10s\n" "$c" "$d" "$e" "$f" "$h" "$g"
        #i=`expr $i + 1`
        let i=i+1
   done
   rm -rf ./[0-9].txt
}
###### MemoryInfo include AEP ######
MemoryInfo_for_AEP(){
   show_produce_message "MemoryInfo"
   local i=1
   slot=`dmidecode -t memory|grep "Memory Device"|wc -l`
   dimm=`dmidecode -t memory|grep "Size"|grep -E "GB|MB|GiB|MiB|TiB"|wc -l`
   #size=`dmidecode -t memory|grep "Size"|grep -E "GB|MB|GiB|MiB|TiB"|awk -F ":" {'print $2'}|grep -i -v "Unknown" |head -1 | awk '{print $1}'`
   unit=`dmidecode -t memory|grep "Size"|grep -E "GB|MB|GiB|MiB|TiB"|awk -F ":" {'print $NF'}|head -1 | awk '{print $NF}'`
   mem_nums=`dmidecode -t memory | grep -B4 'Locator' | grep -v 'Bank' | grep -i size | grep -v 'No' | wc -l`
   ##########################
   #let total_size=mem_nums*size
   #total_size="$total_size $unit"
   #total_size=`ToG "$total_size"`
   dmidecode -t memory|grep "Size"|grep -E "GB|MB"|awk -F ":" {'print $2'}|awk '{print $1,$2}'|sort -nr|uniq -c >total.txt
   unit=`cat total.txt|awk '{print $3}'|sed 's/ //g'` 
   unitL=`echo $unit|wc -L`
   unitl=`cat total.txt|awk '{print $3}'|sed 's/ //g'|wc -l`
   if [ "$unitL" -gt 3 ];then
		for ((m=1; m<=$unitl; m++))
		do
			unit1=`cat total.txt|awk '{print $3}'|sed -n "$m"p`
			if [ $unit1 = MB ];then
				num=`cat total.txt|awk '{print $2}'|sed -n "$m"p`
				num=`expr $num / 1000`
				num11=`cat total.txt|awk '{print $1}'|sed -n "$m"p`
				num1=`expr $num11 \* $num`
			fi
			if [ $unit1 = GB ];then
				num=`cat total.txt|awk '{print $2}'|sed -n "$m"p`
				num21=`cat total.txt|awk '{print $1}'|sed -n "$m"p`
				num2=`expr $num21 \* $num`
			fi
		done
		total_size=`expr $num1 \+ $num2`
		total_size=${total_size}" GB"
   fi
   free -m >freememory_size.txt
   freememory=`cat freememory_size.txt|sed -n 2p|awk -F ":" '{print $2}'|awk '{print $1}'`
   freememory=${freememory}" MB" 
   ##################
   #size="$size $unit"   
   #size=`ToG "$size"`   
   ipmctl show -topology >memtopo.txt
   sed -i '1,3d' memtopo.txt
   cat memtopo.txt |awk -F"|" '{print $2 $3}'|uniq -c >memtopo1.txt
   memtype=`cat memtopo1.txt|wc -l`
   if [ $memtype -ne 1 ];then
	size1=`cat memtopo1.txt |awk '{print $2,$3,$4,$5,$6}'|sed -n 1p`
	memnum1=`cat memtopo1.txt |awk '{print $1}'|sed -n 1p`
	size2=`cat memtopo1.txt |awk '{print $2,$3,$4}'|sed -n 2p`
	memnum2=`cat memtopo1.txt |awk '{print $1}'|sed -n 2p`
	size=${memnum1}":"${size1}"; "${memnum2}":"${size2}
   elif [ $memtype -eq 1 ];then
	size1=`cat memtopo1.txt |awk '{print $2,$3,$4}'`
	memnum1=`cat memtopo1.txt |awk '{print $1}'`
        size=${memnum1}":"${size1}
   fi
   ##################
   freq=`dmidecode -t memory|grep "Configured Clock Speed:"|awk -F: '{print $2}'| grep -i -v "Unknown"|head -1 | sed 's/.//'`
   DDRx=`dmidecode -t memory|grep "Type"|grep -v "Detail"|grep -v "Error Correction Type"| grep -i -v "Unknown" |awk -F: '{print $2}'|head -1 | sed 's/^.//'`
   show_item "Physical Slots:" "$slot"
   show_item "Populated DIMMs:" "$dimm"
   show_item "Populated dimm_size:" "$size"
   #show_item "Memory Total Capacity (DDR4+AEP):" "$total_size"
   show_item "Current Available Memory Capacity:" "$freememory"
   show_item "Freqency:" "$freq"
   show_item "DDRx:" "$DDRx"
   MemoryTopo_for_AEP
   echo "the detail of memory population :"
   echo "==============================="
   dmidecode -t memory|grep "Locator"|grep -v Bank|awk -F: '{print $2}'|sed 's/ //g'>./1.txt
   dmidecode -t memory|grep "Size"|awk -F: '{print $2}'> ./2.txt
   sed -i 's/No Module Installed/No Module/g' 2.txt
   dmidecode -t memory|grep "Manufacturer:"|awk -F: '{print $2}'>./3.txt
   dmidecode -t memory|grep "Part Number:"|awk -F: '{print $2}'>./4.txt
   dmidecode -t memory|grep "Serial Number:"|awk -F: '{print $2}'>./5.txt
   dmidecode -t memory|grep "Configured Clock Speed:"|awk -F: '{print $2}'>./6.txt
   #IsNull Type 7.txt
   #dmidecode -t memory|grep "Type"|grep -v "Detail"|grep -v "Error Correction Type"|awk -F: '{print $2}'>>./7.txt
   ###############
   ipmctl show -topology >./tmp7.txt
   sed -i "1,3d" tmp7.txt
   num_tmp7=`cat tmp7.txt|wc -l`
   for (( a=1; a<=$slot; a++ ))
   do
	x=`cat 1.txt |sed -n "$a"p`
	#col1=${col1:=0}
	for (( j=1; j<=$num_tmp7; j++ ))
	do
		y=`cat tmp7.txt |awk -F"|" '{print $5}' |sed -n "$j"p|sed 's/ //g'`
		#col2=${col2:=0}
		if [[ "$x"w = "$y"w ]];then
			typeAEP=`cat tmp7.txt |awk -F"|" '{print $2}' |sed -n "$j"p`
			echo $typeAEP >>./7.txt
			break
		fi
		typeAEP=1
	done  
		if [[ $typeAEP == 1 ]];then
			echo "Unknown" >>./7.txt
		
		fi
   done  
   ###############
   printf "%-15s%-10s%-10s%-25s%-20s%-10s\n" "Locator" "Size" "Vendor" "Part_Number" "Speed" "Type"
   while [[ $i -le $slot ]]; do
        c=`sed -n "$i p" 1.txt`
        d=`sed -n "$i p" 2.txt`
        e=`sed -n "$i p" 3.txt`
        f=`sed -n "$i p" 4.txt`
        h=`sed -n "$i p" 6.txt`
        g=`sed -n "$i p" 7.txt`
        printf "%-15s%-10s%-10s%-25s%-20s%-10s\n" "$c" "$d" "$e" "$f" "$h" "$g"
        #i=`expr $i + 1`
	let i=i+1
   done
   rm -rf ./[0-9].txt
   rm -rf ./tmp7.txt ./memtopo.txt ./memtopo1.txt ./total.txt
   #show_produce_message "MemoryInfo"
}

installipmctl(){
ipmctl version >/dev/null 2>&1
if [ "$?" != 0 ];then
	yum install ndctl
	rpm -ivh --nodeps --force ./ipmctl/*.rpm
else
	echo "already install ipmctl"
fi
sleep 2
}
MemoryInfo_AEP_base(){
show_produce_message "MemoryInfo of AEP"
#dmidecode -t memory |grep "Type Detail"|awk -F "=" '{print $2}'
show_title "AEPCurrentMode"
#ipmctl show -system -capabilities|grep -E -i "PlatformConfigSupported|Alignment|CurrentVolatileMode" |sed '/^$/d'
ipmctl show -system -capabilities |sed '/^$/d'
show_title "MemoryResources"
ipmctl show -memoryresources|sed '/^$/d'
#show_title "Memory Topology"
#ipmctl show -topology
show_produce_message "MemoryInfo"
}
LOM_or_NIC_1G (){
    ethtool -e $1 &>/dev/null
    if [ "$?" == "0" ]; then
       eeprom_word=`ethtool -e $1 offset 0x7 length 1 | grep 0x0007 | awk '{print $2}' | cut -c 2`
       eeprom_word=`echo $((16#$eeprom_word))`
       if [ ${eeprom_word} -lt 8 ]; then
           return $NIC
       else
           return $LOM
       fi
    fi
}

###################################
LOM_or_NIC_10G ()
{
    ethtool -e $1 &>/dev/null
    if [[ "$?" == "0" ]]; then
       eeprom_word=`ethtool -e $1 offset 0x21 length 1 | grep 0x0021 | awk '{print $2}' | cut -c 2`
       eeprom_word=`echo $((16#$eeprom_word))`
       if [ ${eeprom_word} -lt 8 ]; then
           return $NIC
       else
           return $LOM
       fi
    fi
}
find_nic_name ()
{
  found=0
  for (( i=1; i<${#dev_id[@]}; i=$i+2 ))
  do
     if [ "$1" == "${dev_id[$i]}" ]; then
         nic_name=${dev_id[$((i-1))]}
         found=1
         break
     fi
  done
  if [ $found -eq 0 ]; then
      nic_name="UNKOWN"
  fi
}

EthInfo(){
printf "%-12s%-15s%-8s%-12s%-12s%-15s%-32s%-25s%-20s%-13s%-12s\n" Interface Speed Link Bus "NIC/LOM" "PCIe_slot" "FW" Driver MAC Port Silicn
#define array for keeping all ethernet interfaces
#echo "Interface--Speed--Link--Bus--NIC/LOM--PCIe_slot--Driver--IP--Port--Silicn"
declare -a eth_if
declare -a bus_info
counter=0
#assuming driver is already loaded
interface=`ifconfig -a | grep Link | awk '{print $1}' | sort |  egrep -v 'ib|inet6|lo|bond|virbr0*' `
if [ -z "$interface" ];then
   interface=`ifconfig -a|grep flags|awk -F ":" '{print $1}' | sort |  egrep -v 'ib|inet6|lo|bond|virbr0*'`
fi
for x in $interface
do
   #MAC=$(ifconfig -a $x | grep "Link" | egrep -v inet6\|lo | awk '{print $5}')
   MAC=$(ifconfig -a $x | grep -i "hwaddr" | awk '{print $NF}')
   if [ -z "$MAC" ]; then
      MAC=$(ifconfig -a $x | grep -i ether | awk '{print $2}')
   fi
   if [ -z "$MAC" ]; then MAC="NULL";fi

   SPEED=$(ethtool $x 2>/dev/null | grep "Speed" | cut -d: -f2 | sed 's/^[ \t]*//')
   if [ -z "$SPEED" ]; then SPEED="UNKOWN";fi
   if [[ "$SPEED" =~ Unknown.* ]]; then SPEED="UNKOWN"; fi

   LINK=$(ethtool $x 2>/dev/null | grep "Link detected" | cut -d: -f2 | sed 's/^[ \t]*//')
   if [ -z "$LINK" ]; then LINK="UNKOWN"; fi
   PORT=$(ethtool $x 2>/dev/null | grep "Port" | cut -d: -f2 | sed 's/^[ \t]*//')
   if [ -z "$PORT" ]; then PORT="UNKOWN"; fi
   ethtool -i $x &>/dev/null;
   if [ "$?" == "0" ]; then
       BUS=$(ethtool -i $x | grep bus-info | awk '{print $2}')
       DRIVER=$(ethtool -i $x | grep driver | awk '{print $2}')
       FW=$(ethtool -i $x | grep -i firmware-version | awk -F':' '{print $NF}')
       DRIVER_VER=$(ethtool -i $x | grep "^version" | awk '{print $2}')
   else
       BUS="NULL"
       DRIVER="NULL"
       FW="NULL"
       DRIVER_VER="NULL"
   fi
   case ${DRIVER} in
      e1000|e1000e|igb)
           LOM_or_NIC_1G $x
           if [ "$?" == "0" ]; then
               TYPE="NIC"
           else
               TYPE="LOM"
           fi
           eth_if[$counter]=$x
           bus_info[$counter]=$BUS
           ;;
      ixgbe)
           LOM_or_NIC_10G $x
           if [ "$?" == "0" ]; then
               TYPE="NIC"
           else
               TYPE="LOM"
           fi  
           eth_if[$counter]=$x
           bus_info[$counter]=$BUS
           ;; 
      *)  
         TYPE="UNKOWN"
         eth_if[$counter]=$x
         bus_info[$counter]=$BUS
         ;;  
   esac
   let counter++

   if [[ $BUS =~ [0-9a-f]*:[0-9a-f]*:[0-9a-f]*\.[0-9a-f]* ]]; then 
      PCIE=`lspci -s $BUS -vvv | sed  -n 's/Capabilities: \[a.\] Express (\(.*\)) Endpoint.*/\1/p' | sed 's/^[ \t]*//'`
      if [ -z "$PCIE" ]; then
          PCIEBW=`lspci -s $BUS -vvv | sed -n 's/Link: Speed \(.*\)/\1/p' | sed -n 's/ Width //p' | sed 's/^[ \t]*//'`
          PCIEBW="1.x,`echo $PCIEBW`"
      elif [ "$PCIE" == "v2" ]; then
          PCIEBW="$PCIE,`lspci -s $BUS -vvv | grep "\<LnkSta\>" | sed -n 's/.*:\(.*\),.*/\1/p' | sed -n 's/Speed //p' | sed -n 's/ Width //p' | sed 's/^[ \t]*//'`"
	
      else
          PCIEBW="NULL"
      fi  
      find_nic_name 0x`lspci -s $BUS -n | sed -n 's/.*8086:\([0-9a-f]*\).*/\1/p' | tr "[a-f]" "[A-F]" `
  else
      PCIEBW="NULL"
      nic_name="UNKOWN"
  fi  

   printf "%-12s%-15s%-8s%-13s%-10s%-15s%-32s%-25s%-20s%-15s%-12s\n" $x $SPEED $LINK $BUS $TYPE $PCIEBW "$FW" "$DRIVER-$DRIVER_VER" $MAC "$PORT" $nic_name
   #echo $x"--"$SPEED"--"$LINK"--"$BUS"--"$TYPE"--"$PCIEBW"--"$DRIVER-$DRIVER_VER"--"$IP"--"$PORT"--"$nic_name
done
}
networkinfo()
{
        show_produce_message "Ethernet Detail Message"
	local i
	ethernet_busid=($(cat pcie_device.txt | grep -i ethernet |awk '{print $1}'))
	EthInfo > networkinfo.txt
	for i in ${ethernet_busid[@]}
	do
		lspci -s$i
		if [[ "$i" =~ [0-9a-f]*:[0-9a-f]*\.[0-9a-f]* ]]; then 
			#pcie_flag=`lspci -s$i -vvv | sed  -n 's/Capabilities: \[[ac].\] Express (\(.*\)) Endpoint.*/\1/p' | sed 's/^[ \t]*//'`
			pcie_flag=`lspci -s$i -vvv | grep -i -E 'Capabilities: .* Express' | sed 's/.*(\(.*\)).*/\1/'`
			if [ -z "$pcie_flag" ]; then
				pciebw=`lspci -s$i -vvv | sed -n 's/Link: Speed \(.*\)/\1/p' | sed -n 's/ Width //p' | sed 's/^[ \t]*//'`
			elif [ "$pcie_flag" == "v2" ]; then
				theory=`lspci -s$i -vv | grep "LnkCap:" |sed -n 's/.*\(Speed.*Width....\).*/\1/p'`
				pciebw=`lspci -s"$i" -vv |grep -E "(LnkSta:)" | sed -n 's/.*:\(.*\),.*/\1/p' | sed 's/^[[:blank:]]*\(.*\)/\1/'`
			else
				theory=""
				pciebw=""
			fi
			fw=`cat networkinfo.txt |  grep -i "$i" | uniq |awk -F' {2,}' '{print $7}'`
			driver=`cat networkinfo.txt |  grep -i "$i" | uniq |awk -F' {2,}' '{print $8}'`
		else
			busid=NotMatchStandard
			pciebw=NULL
		fi
		show_item "BUSID" "$i"
		show_item "PCIEBW Theory" "$theory"
		show_item "PCIEBW Reality" "$pciebw"
		show_item "FW" "$fw"
		show_item "DRIVER" "$driver"
	done
	cat networkinfo.txt
	rm -f networkinfo.txt
        show_produce_message "Ethernet Detail Message"
}
ibcard()
{
   local i
   ibcard_busid=($(cat pcie_device.txt | grep -E -i "network controller|Infiniband" |awk '{print $1}'))
   if [ ${#ibcard_busid[@]} -ne 0 ]; then
        show_produce_message "IB Card Detail Message"
   	for i in ${ibcard_busid[@]}
   	do
      	      lspci -s$i
	      lspci -vvv -s $i >devinfo.txt
	      cat devinfo.txt|grep "Device Serial Number"
	      cat devinfo.txt|grep "DevCtl"
	      cat devinfo.txt|grep "LnkSta"
	      cat devinfo.txt|grep "LnkCap"
	      cat devinfo.txt|grep "DevCap"
	      cat devinfo.txt|grep "SR-IOV"
	 done
   	 opainfo=`opainfo | sed '/Xmit Data:/d' | sed '/Recv Data:/d' | sed '/LID:/d'`
	 echo -e "\nopsinfo:\n$opainfo"
   	 show_produce_message "IB Card Detail Message"
    fi 
    rm -rf devinfo.txt
}
function Controller()
{
   controller="$1"
   flag=0
   controller1="2108 2208 3108 3316"
   controller2="2008 2308"
   controller3="3008 3216"

   busid=`echo $controller| awk '{print $1}'`
   for str in $controller1
   do
      res1=`echo $controller|grep -i $str`
      if [[ -n $res1 ]];then
	 flag=1               #storcli
         busid=`echo "$busid" | sed 's/\(.*\)\(\.\)\(.*\)/\1\:\30/'`
         index_line=`storcli /call show all | grep -B7 "$busid" | awk '/^Controller/ {print $NF}'`
         return 0
      else
         continue
      fi
   done
   for str in $controller2
   do
      res2=`echo $controller|grep -i $str`
      if [[ -n $res2 ]];then
	 flag=2              #sas2irc
         busid=`echo "$busid" | sed 's/\(.*\)\(\.\)\(.*\)/\1\:\30/' | sed 's/:/h:/g'`
         index_line=`$Cur_Dir/sas2ircu list | sed -n "/$busid/p"`
         index_line=`echo "$index_line" | awk '{print $1}'`
         return 0
      else
         continue
      fi
   done
   for str in $controller3
   do
      res3=`echo $controller|grep -i $str`
      if [[ -n $res3 ]];then
	 flag=3              #sas2irc
         busid=`echo "$busid" | sed 's/\(.*\)\(\.\)\(.*\)/\1\:\30/' | sed 's/:/h:/g'`
         index_line=`$Cur_Dir/sas3ircu list | sed -n "/$busid/p"`
         index_line=`echo "$index_line" | awk '{print $1}'`
         return 0
      else
         continue
      fi
   done
   res4=`echo $controller| grep -i "Adaptec Series 8"`
   if [[ -z "$res4" ]];then
      show_fail_message "The controller isn't supported.!"
      return 1
   else
      controller="pm8069"
      flag=4              #pm8069
      return 0
   fi
}
get_raid_basic()
{
   local index_raid=$1
   local raid_controller=`echo "$2" | awk -F ":" '{print $3}'`
   local i
   hdd_vendor=`storcli -pdlist --a$index_raid | grep -i "Inquiry Data"|awk -F ":" '{print $2}'|awk '{print $1}'|head -1`
   hdd_type=`storcli -pdlist --a$index_raid | grep -i "PD type"|awk -F ":" '{print $2}'|head -1`
   hdd_num=`storcli -pdlist --a$index_raid | grep -i "PD type"|wc -l`
   
   echo "HDD Vendor:          "$hdd_vendor
   echo "HDD Type:            "$hdd_type
   echo "HDD RPM:              unknown"
   echo "HDD PCS:              "$hdd_num
   echo "RAID controller   :  "$raid_controller
   storcli -pdlist --a$index_raid | grep -i "Inquiry Data"|awk -F ":" '{print $2}'|awk '{print $1}'>1.txt
   storcli -pdlist --a$index_raid | grep -i "PD type"|awk -F ":" '{print $2}'>2.txt
   storcli /c$index_raid/eall/sall show all|grep -i SN|awk -F "=" '{print $2}'>>3.txt
#This command shows the summary information for all the enclosures and physical drives connected to the controller.Input example:
   i=1
   hdd_num=`storcli -pdlist --a$index_raid | grep -i "PD type"|wc -l`
   printf "%-10s%-30s%-30s%-10s\n" "number" "HDD Vendor" "HDD SN" "HDD Type"
   while [[ $i -le $hdd_num ]];do
      vendor=`sed -n "$i p" 1.txt|sed 's/^[[:space:]]*//g'`
      type=`sed -n "$i p" 2.txt|sed 's/^[[:space:]]*//g'`
      sn=`sed -n "$i p" 3.txt|sed 's/^[[:space:]]*//g'`
      printf "%-10s%-30s%-30s%-10s\n" "$i" "$vendor" "$sn" "$type"
      #i=`expr $i + 1`
      let i=i+1
   done
   rm -rf 1.txt 2.txt 3.txt
}
errinfo(){
	show_produce_message "ErrorInfo"
	#get os err
 	show_title "Message Error" | tee message_mce_sel.log
	cat /var/log/messages | grep -i error | tee message_mce_sel.log
	cat /var/log/messages | grep -i fail  | tee -a message_mce_sel.log
	cat /var/log/messages | grep -i fault | tee -a message_mce_sel.log
	cat /var/log/messages | grep -i warning | tee -a message_mce_sel.log
	if [ -s /var/log/mcelog ]; then
		show_title "Mce Error" | tee -a message_mce_sel.log
		cat /var/log/mcelog | grep -i error  | tee -a message_mce_sel.log
		cat /var/log/mcelog | grep -i fault | tee -a message_mce_sel.log
		cat /var/log/mcelog | grep -i warning | tee -a message_mce_sel.log
		cat /var/log/mcelog | grep -i fail | tee -a message_mce_sel.log
		cat /var/log/mcelog | grep -i ecc | tee -a message_mce_sel.log
	fi
	ipmitool sel elist | tee -a message_mce_sel.log
	show_title "RAID Error"
	local i
	local j
	echo "Err Info :"
   	echo "========"
	storcli -pdlist -aALL|grep -i "Inquiry Data"|awk -F ":" '{print $2}'>1.txt
   	storcli -pdlist -aALL|grep -i "link speed"|awk -F ":" '{print $2}'>2.txt
   	storcli -pdlist -aALL|grep -i "Firmware state"|awk -F ":" '{print $2}'>3.txt
   	storcli -pdlist -aALL|grep -i "Media Error Count"|awk -F ":" '{print $2}'>4.txt
   	storcli -pdlist -aALL|grep -i "Other Error Count"|awk -F ":" '{print $2}'>5.txt
   	hdd_num=`storcli -pdlist -aALL|grep -i "Inquiry Data"|wc -l`
   	i=1
	printf "%-20s%-20s%-20s%-20s%-20s\n" "Inquiry_Data" "link_speed" "Firmware_state" "Media_Error_Count" "Other_Error_Count"
	#echo "Inquiry Data--link speed--Firmware state--Media Error Count--Other Error Count"
	while [[ $i -le $hdd_num ]];do
            inquiry_data=`sed -n "$i p" 1.txt|sed 's/^[[:space:]]*//g'`
            link_speed=`sed -n "$i p" 2.txt|sed 's/^[[:space:]]*//g'`
            fw_state=`sed -n "$i p" 3.txt|sed 's/^[[:space:]]*//g'`
            media_err=`sed -n "$i p" 4.txt|sed 's/^[[:space:]]*//g'`
            other_err=`sed -n "$i p" 5.txt|sed 's/^[[:space:]]*//g'`
            printf "%-20s%-20s%-20s%-20s%-20s\n" "$inquiry_data" "$link_speed" "$fw_state" "$media_err" "$other_err"
            #echo $inquiry_data"--"$link_speed"--"$fw_state"--"$media_err"--"$other_err
            #i=`expr $i + 1`
            let i=i+1
        done
        rm -rf 1.txt 2.txt 3.txt 4.txt 5.txt
        echo "  "
   	#echo "pall phyerrorcounters :"
   	#echo "========"
	#echo "Loops ---------->"$loop >> $LogAd/ROC_Tem.log
   	#echo "ROC temperature :" >>$LogAd/ROC_Tem.log
   	#echo "==============="
   	#storcli -AdpAllInfo -a0|grep -i "ROC temp"|awk -F ":" '{print $2}' >> $LogAd/ROC_Tem.log
   	raid_num=`storcli show ctrlcount | awk -F'=' '/Controller Count/ {print $2}' | sed 's/[[:space:]]//g'`
   	for((i=0;i<$raid_num;i++))
   	do	
	    storcli /c"$i"/pall show all | sed -n '/Phy_No Invld_Dwrd_Cnt/,$p'
            echo "eall phyerrorcounters :"
            echo "====================="
            storcli /c"$i"/eall show phyerrorcounters|sed '/Controller = 0/d'|sed '/Status = Success/d'|sed '/Description = None/d'|sed '/^ *&/d'
            echo "sall phyerrorcounters :"
            echo "====================="
            storcli /c"$i"/eall/sall show phyerrorcounters|sed '/Controller = 0/d'|sed '/Status = Success/d'|sed '/Description = None/d'|sed '/^ *&/d'
        done
	show_title "Free-State"
	free -h
	show_title "Fan Speed"
	ipmitool sensor list | grep -i rpm
	show_title "SDR List"
	ipmitool sdr elist
	show_title "P-State"
	cpupower monitor
	turbostat &
	sleep 5 
	killall -9 turbostat >/dev/null
	show_title "PCIE Register Dump"
	for i in ${pcie_device_busid}
	do
		if [[ "$i" =~ [0-9a-f]*:[0-9a-f]*\.[0-9a-f]* ]]; then 
			lspci -s"$i" -xxx
  		fi  
	done
#        rm -rf *.txt
	rm -rf *.sn
	show_produce_message "ErrorInfo"
}
raidinfo(){

    #bbu
    #storcli /c$1/bbu show all | sed -n '/Detailed Status/,$p' | sed '/^$/d' >raid_bbu.temp
    #raid detail message
    storcli /c"$1" show |sed '/Generating detailed summary/d'|sed '/Controller = 0/d'|sed '/Status = Success/d'|sed '/Description = None/d'|sed '/^ *&/d' >raid_info
:<<!
    Product_Name=`cat raid_info | grep -i 'Product Name' | awk -F'=' '{print $NF}' | sed 's/^[[:space:]]*//'`
    Serial_Number=`cat raid_info | grep -i 'Serial Number' | awk -F'=' '{print $NF}'| sed 's/^[[:space:]]*//'`
    SAS_Address=`cat raid_info | grep -i 'SAS Address'| awk -F'=' '{print $NF}'| sed 's/^[[:space:]]*//'`
    PCI_Address=`cat raid_info | grep -i 'PCI Address'| awk -F'=' '{print $NF}'| sed 's/^[[:space:]]*//'`
    BIOS_Version=`cat raid_info | grep -i 'BIOS Version'| awk -F'=' '{print $NF}'| sed 's/^[[:space:]]*//'`
    FW_Version=`cat raid_info | grep -i 'FW Version'| awk -F'=' '{print $NF}'| sed 's/^[[:space:]]*//'`
    Driver_Name=`cat raid_info | grep -i 'Driver Name'| awk -F'=' '{print $NF}'| sed 's/^[[:space:]]*//'`
    Driver_Version=`cat raid_info | grep -i 'Driver Version'| awk -F'=' '{print $NF}'| sed 's/^[[:space:]]*//'`
    show_item "Product_Name"  "$Product_Name"
    show_item "Serial_Number" "$Serial_Number"
    show_item "SAS_Address"   "$SAS_Address"
    show_item "PCI_Address"   "$PCI_Address"
    show_item "BIOS_Version"  "$BIOS_Version"
    show_item "FW_Version"    "$FW_Version"
    show_item "Driver_Name"   "$Driver_Name"
    
    show_title "BBU Status"
    cat raid_bbu.temp
!
    ##########################TOPOLOGE########################
    sed -n '/TOPOLOGY/,/DG=Disk Group Index/p' raid_info|sed '$d'| awk '{printf "%-10s%-7s%-7s\n", $4, $6,$7;}'>1temp
    ####sed size###
    sed -n '/TOPOLOGY/,/DG=Disk Group Index/p' raid_info|sed '$d'|awk '{print $9$10}'|sed 's/SizePDC/Size/g'>size.temp
    ###sed PDC##
    sed -n '/TOPOLOGY/,/DG=Disk Group Index/p' raid_info|sed '$d'|awk '{print $11 }'|sed 's/PI/PDC/g'>PDC.temp
    paste size.temp PDC.temp|awk '{printf "%-12s%-6s\n",$1,$2}'>2temp
    paste 1temp 2temp >TOPOLOGE.temp
    sed -i '1,3d' TOPOLOGE.temp
    if [ -s TOPOLOGE.temp ]; then
	    show_title "TOPOLOGE  :"
	    echo "======="
	    echo "-------------------------------------------------------------------------"
	    cat TOPOLOGE.temp|sed '/^$/d'
	    echo "-------------------------------------------------------------------------"
    fi
    rm *temp -rf
    ##########################VD LIST ########################
    local i=0
    vd_nums=`storcli /c"$1"/vall show | sed -n '/DG\/VD/,/Cac=CacheCade|Rec=Recovery/'p | sed '$d' | sed '/^$/d' | sed '1,2d' | sed '$d' | wc -l`
    if [ $vd_nums -gt 0 ]; then
	    show_title "Virtual Drives :"
   	    echo "=============="
	    for((i=0;i<$vd_nums;i++))
    	    do
		storcli /c"$1"/v"$i" show all | sed -n '/DG\/VD/,/Cac=CacheCade/p' | sed '$d' | sed '/^$/d' >> vd.temp
		storcli /c"$1"/v"$i" show all | sed -n '/PDs/,/EID-Enclosure/p' | sed '$d' | sed '/^$/d' >> vd.temp
   	    done
    	    if [ -s vd.temp ]; then
	        cat vd.temp
       	    fi
    	    rm *temp -rf
    fi
    #########################PD LIST#########################
    sed -n '/PD LIST/,/EID-Enclosure Device ID/p' raid_info|sed '$d' |awk '{print $5$6}'|sed 's/SizeIntf/Size/g'>size.temp
    sed -n '/PD LIST/,/EID-Enclosure Device ID/p' raid_info|sed '$d' |awk '{print $11}'|sed 's/Model/SeSz/g'>sesz.temp
    sed -n '/PD LIST/,/EID-Enclosure Device ID/p' raid_info|sed '$d' |awk '{print $12}'|sed 's/Sp/Model/g'>model.temp
    sed -n '/PD LIST/,/EID-Enclosure Device ID/p' raid_info|sed '$d'|awk '{printf "%-8s%-7s\n", $1,$3;}'|sed 's/PD/PD LIST/g'>1temp
    paste size.temp sesz.temp model.temp |awk '{printf "%-12s%-6s%-12s\n",$1,$2,$3}'>2temp
    paste 1temp 2temp >PD_LIST.temp
    if [ -s PD_LIST.temp ]; then
	    show_title "PD LIST :"
	    echo "======="
	    cat PD_LIST.temp | sed '1,4d'
    fi
    rm *temp -rf
    rm raid_bbu* -rf
    rm raid_info -rf
}
get_raid_message()
{
	local index="$1"
        local raidmesg="$2"
	local state

	if [ $flag -eq 1 ];then
	      #num=`storcli /c0 show|grep -i "Physical Drives"|awk -F '=' {'print $2'}`
	      #echo "Numbers of disk:   " $num
	      #echo "Physical Drives:   "
	      show_title "Versions"
	      storcli -AdpAllInfo -a$index | grep -i ^adapt
	      storcli -AdpAllInfo -a$index | sed -n '/Product Name/p'
	      storcli -AdpAllInfo -a$inded | sed -n '/Serial No/p'
	      storcli -AdpAllInfo -a$index | sed -n '/FW Package Build/p'
	      storcli -AdpAllInfo -a$index | sed -n '/BIOS Version/p'
	      storcli -AdpAllInfo -a$index | sed -n '/Ctrl-R Version/p'
	      storcli -AdpAllInfo -a$index | sed -n '/Preboot CLI Version/p'
	      storcli -AdpAllInfo -a$index | sed -n '/FW Version/p'
	      storcli -AdpAllInfo -a$index | sed -n '/NVDATA Version/p'
	      storcli -AdpAllInfo -a$index | sed -n '/Boot Block Version/p'

	      sc_status=`storcli /c$index/cv show all | sed -n '/^Status/p' | awk '{print $NF}'`
              show_title "RAID Super capacitor"
	      if [[ "$sc_status" != "Failure" ]]; then
                      storcli /c$index/cv show all | sed -n '/Type/p'
                      state=`storcli /c$index/cv show all | sed -n '/State/p' | awk '{print $NF}' | tr '[A-Z]' '[a-z]'`
                      storcli /c$index/cv show all | sed -n '/State/p'
                      storcli /c$index/cv show all | sed -n '/CacheVault Flash Size/p'
                      temperature_message=`storcli /c$index/cv show all | sed -n '/Temperature/p'`
                      temperature=`echo "$tempperature_message" | awk '{print $2}'`
                      if [[ $temperature -gt $Thresholdtemp ]] || [[ "$state" =~ "fail" ]]; then
                         echo "$temperature_message"
                         show_title "RAID Super capacitor All Message"
                         storcli /c$index/cv show all
                      fi
              else
                      storcli /c$index/cv show all
              fi

	      show_title "RAID HW Configure"
	      storcli -AdpAllInfo -a$index | sed -n '/HW Configuration/,/ROC temperature/p' | sed '1,2d'
		
	      show_title "RAID Policy"	      
	      storcli -cfgdsply -aALL |grep Policy

	      show_title "RAID/HBA Link HDD:"	
	      get_raid_basic $index $raidmesg

	      show_title "controller info detail :"
	      raidinfo $index
   	fi
	if [ $flag -eq 2 ];then
 		#./sas2ircu $index DISPLAY 
	      	#echo "SAS-HBA/RAID:         RAID"
		show_titel "Controller information"
		$Cur_Dir/sas2ircu $index DISPLAY | sed -n '/Controller information/,/IR Volume information/p' | sed '1d' | sed '$d'
		ir_flag=`$Cur_Dir/sas2ircu $index DISPLAY | sed -n '/IR Volume information/,/Physical device information/p' | sed '1d' | sed '$d' | wc -l`
		if [ $ir_flag -gt 2 ]; then
			show_title "IR Volume information"
			$Cur_Dir/sas2ircu $index DISPLAY | sed -n '/IR Volume information/,/Physical device information/p' | sed '1d' | sed '$d'
		fi
		show_title "Physical device information"
		$Cur_Dir/sas3ircu $index DISPLAY | sed -n '/Physical device information/,/Enclosure information/p' | sed '/Initiator at ID/d' |sed '1d' | sed '$d'
	fi
	if [ $flag -eq 3 ];then
		show_title "Controller information"
		$Cur_Dir/sas3ircu $index DISPLAY | sed -n '/Controller information/,/IR Volume information/p' | sed '1d' | sed '$d'
		
		ir_flag=`$Cur_Dir/sas3ircu $index DISPLAY | sed -n '/IR Volume information/,/Physical device information/p' | sed '1d' | sed '$d' | wc -l`
		if [ $ir_flag -gt 2 ]; then
			show_title "IR Volume information"
			$Cur_Dir/sas3ircu $index DISPLAY | sed -n '/IR Volume information/,/Physical device information/p' | sed '1d' | sed '$d'
		fi
		show_title "Physical device information"
		$Cur_Dir/sas3ircu $index DISPLAY | sed -n '/Physical device information/,/Enclosure information/p' | sed '/Initiator at ID/d' |sed '1d' | sed '$d'
    		#./sas3ircu $index DISPLAY
	        #echo "SAS-HBA/RAID:         RAID"
   	fi
}

raidcard()
{
	local i=0
	local c
	cat pcie_device.txt | grep -E -i "raid|lsi" > raid_controller.txt
        hdd_num=`fdisk -l 2>/dev/null|grep dev|grep Disk|grep -v "mapper\|nvme"|sort|wc -l`
	if [ -s raid_controller.txt ]; then
	    	show_produce_message "RAID/HBA Card Info"
		raid_nums=`cat raid_controller.txt | wc -l`
		for((i=1;i<=$raid_nums;i++))
                do
                        raid=`sed -n "$i"p raid_controller.txt `
                        busid=`echo "$raid" | awk '{print $1}'`
                        controller_vendor=`echo $raid|awk -F "/" '{print $1}'`
                        controller=`echo $raid |awk -F "/" '{print $2}'`
                        Controller "$raid"
                        let c=index_line
                        show_produce_message "RAID/HBA Card c$c"
                        show_item "Controller vendor:"    "$controller_vendor"
                        show_item "controller:"          "$controller"
			get_raid_message $c "$raid"

                        show_produce_message "RAID/HBA Card c$c"

                done
	    	show_produce_message "RAID/HBA Card Info"
	fi
	rm -f pcie_device.txt
	rm -f raid_controller.txt
}
pcieinfo()
{
        local i
        ignore="System|bridge|Performance|PIC|SATA|USB|Unassigned|Communication|Signal|Memory|VGA|SMBus|SPI|Encryption|IOMMU|Non-Essential"
        pcie_device_busid=`lspci | grep -i -E -v "$ignore" | awk '{print $1}'`
        for i in ${pcie_device_busid}
        do
                i=`echo "$i" | awk -F'.' '{print $1}'`
                ((pcie_addr["$i"]++))
        done
        add_message=`lspci | grep -E "Bridge: PLX|baba|Non-Volatile memory"`
        add_busid=($(echo "$add_message" | awk '{print $1}'))
        rm -f pcie_device.txt >/dev/null
        show_produce_message "PCIE Devic Message"
        for i in ${pcie_device_busid}
        do
                if [[ "$i" =~ [0-9a-f]*:[0-9a-f]*\.[0-9a-f]* ]]; then
                        lspci -s"$i" | tee -a pcie_device.txt
                        theory=`lspci -s$i -vv | grep "LnkCap:" |sed -n 's/.*\(Speed.*Width....\).*/\1/p'`
                        pciebw=`lspci -s"$i" -vv |grep -E "(LnkSta:)" | sed -n 's/.*:\(.*\),.*/\1/p' | sed 's/^[[:blank:]]*\(.*\)/\1/'`
                        show_item "PCIEBW Theory" "$theory"
                        show_item "PCIEBW Reality" "$pciebw"
                fi
        done
        if [[ "${add_busid[@]}" != "" ]]; then
                for temp_add in ${add_busid[@]}
                do
                        lspci -s"$temp_add" | tee -a pcie_device.txt
                        theory=`lspci -s$temp_add -vv | grep "LnkCap:" |sed -n 's/.*\(Speed.*Width....\).*/\1/p'`
                        pciebw=`lspci -s"$temp_add" -vv |grep -E "(LnkSta:)" | sed -n 's/.*:\(.*\),.*/\1/p' | sed 's/^[[:blank:]]*\(.*\)/\1/'`
                        show_item "PCIEBW Theory" "$theory"
                        show_item "PCIEBW Reality" "$pciebw"
                done
        fi
        show_produce_message "PCIE Devic Message"
}
gpuinfo()
{
    local i
    local j=1
    lspci|grep -i -E "3D controller|NVIDIA Corporation">tmp.txt
    if [ -s tmp.txt ]; then
        show_produce_message "GPU Message"
        echo -n "GPU:   "
        echo ""
        gpus=`cat tmp.txt|awk '{print $1}'`
        for i in $gpus
        do
            sed -n "$j"p tmp.txt
            lspci -s $i -vvvv|grep -i LnkCap
            lspci -s $i -vvvv|grep -i LnkSta
            let j=j+1
            echo ""
        done
    else
        echo "There is not exist GPU in this machine"     
    fi
    #nvidia-smi
    if [ -s tmp.txt ]; then
        nvidia-smi -L
        mn=`nvidia-smi -q | grep -i 'Attached GPUs' | awk '{print $NF}'`
        for((i=0;i<$mn;i++))
        do
        nvidia-smi -q | grep -i -E -B13 -A144 "Minor Number.*$i" >$i.txt
        gpu_module_index=`head -n1 "$i.txt"`
        head -n1 $i.txt
        PN=`cat $i.txt | grep "Product Name" | awk '{print $NF}'`
        Serial=`cat $i.txt | grep -i "Serial" | awk '{print $NF}'|sed 's/[[:space:]]*//'`
        GPU_UUID=`cat $i.txt | grep "GPU UUID" | awk '{print $NF}'|sed 's/[[:space:]]*//'`
        VBIOS=`cat $i.txt | grep "VBIOS" | awk '{print $NF}'|sed 's/[[:space:]]*//'`
        Image_Version=`cat $i.txt | grep "Image Version" | awk '{print $NF}'|sed 's/[[:space:]]*//'`
        Bus_Id=`cat $i.txt | grep "Bus Id" | awk '{print $NF}'|sed 's/[[:space:]]*//'`
        Current=`cat $i.txt | grep "Current"|grep x | awk '{print $NF}'|sed 's/[[:space:]]*//'`
        show_item "Product Name" "$PN"
        show_item "Serial" "$Serial"
        show_item "GPU UUID" "$GPU_UUID"
        show_item "VBIOS" "$VBIOS"
        show_item "Image Version" "$Image_Version"
        show_item "Bus Id" "$Bus_Id"
        show_item "Current" "$Current"
        echo ""
        done
        show_produce_message "GPU Message"
   fi
   rm -f [0-4].txt
   rm -f tmp.txt
}

isnvme()
{	
	echo "NVME: "
	while read line
	do
		echo "The NVME: $line "
      		echo "FW info:"
		nvme fw-log $line
      		echo "SMART info:"
		nvme smart-log $line | sed -n '1,2p'
      		echo "ID info:"
		nvme id-ctrl $line
      		echo "Error info:"
		nvme error-log $line
	done < nvme.temp
}
get_hdd_smart()
{
	  sleep 15
      show_produce_message "HDD Smart Message"
      local i
      local j
      local k
      local l
      local m
      raid_num=`storcli show ctrlcount | awk -F'=' '/Controller Count/ {print $2}' | sed 's/[[:space:]]//g'`
      #fdisk -l 2>/dev/null|grep dev|grep Disk|grep -v "mapper"|sort > disk.txt
      nvme list | sed -n '/\/dev/p' | awk '{print $1}' | sed 's/..$//'| sed '/^$/d' > nvme.temp
      nvme_nums=`cat nvme.temp | wc -l`
      if [ ! -s "nvme.temp" ]; then
          echo "The board not exist the NVME storage type"
      else
          isnvme
      fi
      rm *.temp -rf
      if [ "$flag" = "0" ]; then
           echo "SAS-HBA/RAID:         HBA"
      fi
      raid_hdds=${#raid_hdds_sn[*]}
      if [ $raid_hdds -gt 0 ]; then
        for((m=0;m<$raid_num;m++))
        do
                storcli -pdlist -a$m |grep Id |awk '{print $3}' >> raid_hdd_id.txt
        done
        raid_hdd_id=($(cat raid_hdd_id.txt))
        if [ ${#raid_hdd_id[@]} -gt 0 ]; then
                suffix_flag=`smartctl -s on -i -d megaraid,${raid_hdd_id[0]} /dev/ | grep -i 'failed: INQUIRY failed'`
                if [ "$suffix_flag" == "" ]; then
                        suffix="/dev/"
                fi
                suffix_flag=`smartctl -s on -i -d megaraid,${raid_hdd_id[0]} /dev/sda | grep -i 'failed: INQUIRY failed'`
                if [ "$suffix_flag" == "" ]; then
                        suffix="/dev/sda"
                fi
        fi
        for((m=0;m<$raid_num;m++))
        do
                storcli -pdlist -a$m |grep Id |awk '{print $3}' > raid_hdd_id.txt
                show_title "RAID CARID $m"
                raid_hdd_id=($(cat raid_hdd_id.txt))
                for l in ${raid_hdd_id[@]}
                do
                        slot_num=`storcli -pdlist a$m | grep -B5 -i "device id: $l" | grep -i 'slot' | uniq |sed 's/.*:[[:space:]]*\([0-9]*\)/\1/' | uniq`
                        dev_capability=`smartctl -s on -i -d megaraid,$l "$suffix"|sed '/Local/d' | grep -i 'capacity' | sed 's/.*\(\[.*\]\)/\1/'`
                        dev_type=`smartctl -s on -i -d megaraid,$l "$suffix"| grep -i 'device type' | sed 's/.*:[[:space:]]*\(.*\)/\1/'`
                        ssd_type=`smartctl -s on -i -d megaraid,$l "$suffix"| grep -i 'Rotation Rate' | awk -F':' '{print $2}'`
                        if [[ "$dev_type" != "disk" ]] && [[ "$ssd_type" == "Solid State Device" ]]; then
                                dev_type="SSD"
                                dev_rpm="unknow"
                        else
                                dev_type="HDD"
                        fi
                        if [ "$dev_type" == "HDD" ]; then
                                dev_rpm=`smartctl -s on -i -d megaraid,$l "$suffix"| grep -i 'rotation' | sed 's/.*:[[:space:]]*\([0-9]*\).*/\1/'`
                                #dev_rpm=`expr $dev_rpm / 1000`"k"
  				dev_rpm=`echo "$dev_rpm / 1000" | bc`"k"

                        fi
                        dev_mode=`smartctl -s on -i -d megaraid,$l "$suffix"| grep -i 'transport' | sed 's/.*:[[:space:]]*\([a-zA-Z]*\)/\1/'`
                        if [ "$dev_mode" == "" ]; then
                                dev_mode=`smartctl -s on -i -d megaraid,$l "$suffix"| grep -i 'sata' | sed 's/.*:[[:space:]]*\([a-zA-Z]*.*\),.*/\1/'`
                        fi
                        show_produce_message "disk slot number: $slot_num capability: $dev_capability type: $dev_type mode: $dev_mode rotation_rate: $dev_rpm"
                        smartctl -s on -i -d megaraid,$l "$suffix"| sed '/Local/d'
                        sn=`smartctl -s on -i -d megaraid,$l "$suffix"| grep -i 'serial number' | sed 's/.*:[[:blank:]]*\([0-9a-zA-Z]*\)/\1/'`
                        model=`smartctl -s on -i -d megaraid,$l "$suffix"| grep -E -i 'device model|product' | sed 's/.*:[[:blank:]]*\([0-9a-zA-Z]*\)/\1/' | sed 's/ /_/g' |sed 's/\//_/g'`
                        smartctl -s on -x -d megaraid,$l "$suffix" >>$hdd_path/"$model"_$sn.txt
                done
        done
      fi
      if [ ${#sys_hdd_backup[@]} -gt 0 ]; then
        for j in ${sys_hdd_backup[@]}
        do
                line="`fdisk -l 2>/dev/null|grep dev|grep Disk|grep -v "mapper\|nvme"|sort|grep -i "$j"`"
                if [ "$line" != "" ];then
                dev_capability=`echo $line|awk -F ":" {'print $2'}|awk {"print $2"}|awk -F "," {'print $1'}`
                dev_type_tmp=`cat /sys/block/$j/queue/rotational 2>/dev/null`
                if [[ $dev_type_tmp == "1" ]];then
                        dev_type="hdd"
                        dev_rpm=`sginfo -g /dev/$j|grep -i "Rotational Rate"|awk -F " " '{print $3}' 2>/dev/null`
			dev_mode=`smartctl -s on -i -d megaraid,$l "$suffix"| grep -i 'transport' | sed 's/.*:[[:space:]]*\([a-zA-Z]*\)/\1/'`
                        if [ "$dev_mode" == "" ]; then
                                dev_mode=`smartctl -s on -i -d megaraid,$l "$suffix"| grep -i 'sata' | sed 's/.*:[[:space:]]*\([a-zA-Z]*.*\),.*/\1/'`
                        fi
                        if [[ $dev_rpm == ""  ]];then
                                #dev_rpm=`hdparm -I /dev/$j|grep -i "Rotation Rate"|awk -F ":" '{print $2}' 2>/dev/null`
                                dev_rpm=`smartctl -i /dev/$j|grep -i "Rotation Rate"|awk -F ":" '{print $2}' 2>/dev/null`
                                #dev_rpm=`expr $dev_rpm / 1000`"k"
				dev_rpm=`echo "$dev_rpm / 1000" | bc`"k"
                        else
                                #dev_rpm=`expr $dev_rpm / 1000`"k"
				dev_rpm=`echo "$dev_rpm / 1000" | bc`"k"

                        fi
                elif [[ $dev_type_tmp == "0" ]];then
                        dev_type="ssd"
                        dev_rpm="unknown"
                else
                        dev_type="unknown"
                        dev_rpm="unknown"
                fi
                #echo "disk:" $j "capability:" $dev_capability "type:" $dev_type "mode:" $dev_mode "rotation_rate:"$dev_rpm
                show_produce_message "disk: $j capability: $dev_capability type: $dev_type mode: $dev_mode rotation_rate:$dev_rpm"
                smartctl -i /dev/$j | sed '/Local/d'
                sn=`smartctl -i /dev/$j | grep -i 'serial number' | sed 's/.*:[[:blank:]]*\([0-9a-zA-Z]*\)/\1/'`
                model=`smartctl -i /dev/$j | grep -E -i 'device model|product' | sed 's/.*:[[:blank:]]*\([0-9a-zA-Z]*\)/\1/' | sed 's/ /_/g'|sed 's/\//_/g'`
                smartctl -x /dev/$j >> $hdd_path/"$model"_$sn.txt
                #hdparm -I /dev/$j >> $hdd_path/"$model"_$sn.txt
                sdparm  /dev/$j >> $hdd_path/"$model"_$sn.txt
        #       hdd_pan=`echo "$j" d| sed 's/.*\/\([a-z]*\)/\1/'`
                udevadm info --query=all --path=/sys/block/$j >> $hdd_path/"$model"_$sn.txt
                fi
        done
      fi
      rm -rf disk.txt
      rm -rf *.sn
      rm -f raid_hdd_id.txt
      show_produce_message "HDD Smart Message"
}
usb_info()
{
        show_produce_message "USB Info Message"
        lsusb 
        show_produce_message "USB Info Message"
}
scelnxmessage()
{
        show_produce_message "SCELNX Info Message"
        ./SCELNX_64 /o /l mylist.txt.original /n nvram.txt /h hii.txt >/dev/null
        cat $Cur_Dir/nvram.txt | sed '3d'
        show_produce_message "SCELNX Info Message"
        rm -f nvram.txt
}
psuinfo()
{
    psu_num=`ipmitool sdr elist | grep -i -E 'psu.*_status'| awk '/detected/ {print NR}' | tail -n1`
    local i
    if [[ $psu_num != "" ]]; then 
        show_produce_message "PSU Info"
        for((i=0;i<$psu_num;i++))
        do   
            psu_mode=`ipmitool raw 0x3a 0x27 0x06 0x0$i 0x00 0x00 | sed 'H;$!d;g;s/\n/ /g' | cut -d ' ' -f 67-83| xxd -r -p`
            show_item "PSU$i Mode" "$psu_mode"
        done 
        show_produce_message "PSU Info"
    else 
        show_fail_message "ipmitool sdr elist not find PSU module"
    fi   
}

machine_info_check_tool_ver
machineSummary
os_board_info
upiinfo
cpuinfo
########## Judge whether include AEP ###########
ipmctl show -topology >/dev/null 2>&1
if [ "$?" != 0 ];then
	MemoryInfo
else
	installipmctl
	MemoryInfo_for_AEP
	MemoryInfo_AEP_base
fi
sleep 2
###########
pcieinfo
#gpuinfo
networkinfo
ibcard
raidcard
get_hdd_smart
usb_info
psuinfo
errinfo
