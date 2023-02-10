#!/bin/bash
Cur_Dir=$(cd "$(dirname "$0")";pwd)
install_Dir=$Cur_Dir/ins
error_flag=0

cd $Cur_Dir
if [ -d "hdparm-9.50" ]; then
   cd hdparm-9.50 >/dev/null
   make  
   make install  
else
   tar -xf hdparm-9.50.tar.gz	
   cd hdparm-9.50 >/dev/null
   make  
   make install  
fi 
cd $Cur_Dir >/dev/null
if [ -d "nvme-cli-master" ]; then
   cd  nvme-cli-master >/dev/null
   make
   make all  
   make install  
else
   unzip nvme-cli-master.zip
   cd  nvme-cli-master >/dev/null
   make
   make all  
   make install  
fi 
cd $Cur_Dir >/dev/null
if [ -d "smartmontools-6.5" ]; then
   cd  smartmontools-6.5  >/dev/null
  ./configure 
   make
   make all  
   make install  
   cp -f smartctl /usr/sbin/smartctl 
   cp -f smartctl /sbin/smartctl 
   cp -f smartctl /bin/smartctl 
else
   tar -xf smartmontools-6.5.tar.gz
   cd  smartmontools-6.5  >/dev/null
  ./configure 
   make
   make all  
   make install  
   cp -f smartctl /usr/sbin/smartctl 
   cp -f smartctl /sbin/smartctl 
   cp -f smartctl /bin/smartctl 
fi 
cd $Cur_Dir >/dev/null
if [ -d "sdparm-1.10" ]; then
   cd  sdparm-1.10  >/dev/null
  ./configure 
   make
   make all  
   make install  
   cp -f src/sdparm  /usr/sbin/sdparm
   cp -f src/sdparm /sbin/sdparm
   cp -f src/sdparm /bin/sdparm 
else
   tar -xf sdparm-1.10.tgz
   cd  sdparm-1.10  >/dev/null
  ./configure 
   make
   make all  
   make install  
   cp -f src/sdparm  /usr/sbin/sdparm
   cp -f src/sdparm /sbin/sdparm
   cp -f src/sdparm /bin/sdparm 
fi 
cd $Cur_Dir >/dev/null
if [ -d "msr-tools-1.2" ]; then
   cd  msr-tools-1.2  >/dev/null
   make
   make install  
else
   tar -xf msr-tools-1.2.tar.bz2
   cd  msr-tools-1.2  >/dev/null
   make
   make install  
fi 
cd $Cur_Dir >/dev/null
rpm -ivh  storcli-1.21.06-1.noarch.rpm 2>/dev/null
ln -sf /opt/MegaRAID/storcli/storcli64 /usr/bin/storcli
file_type()
{
        keyword=$2
        ins_path="$install_Dir/$keyword"
        if [ ! -d "$ins_path" ]; then
                mkdir -p $ins_path
        else
                rm -rf $ins_path/*
        fi
        compress_type=`file $1 | awk -F':' '{print $2}' | awk -F',' '{print $1}' `
        compress_type=`echo "$compress_type" | tr 'A-Z' 'a-z'`
        if [[ "$compress_type" =~ "gzip" ]] ||[[ "$compress_type" =~ "bzip2" ]] ||[[ "$compress_type" =~ "tar.gz" ]] || [[ "$compress_type" =~ "tar" ]]; then
                uncompress_tool="tar -xf $1 -C $ins_path"
        elif [[ "$compress_type" =~ "zip" ]]; then
                uncompress_tool="unzip $1 -d $ins_path"
        elif [[ "$compress_type" =~ "directory" ]]; then
                uncompress_tool="dir"
                dir="true"
        else
                echo "wrong file type,please check"
                exit 1
        fi
        $uncompress_tool
        if [ ${dir:-false} == "false" ]; then
                src_path=$ins_path
        else
                src_path=$1
        fi
}

install_driver()
{
    local i
    get_gpu_version=`lspci|grep -i -E "3D controller|NVIDIA Corporation" | sed 's/.*compatible controller:\(.*\)(.*/\1/' |uniq |sed 's/.*\(\[.*\]\).*/\1/' | awk '{print $2}' | sed 's/.$//'`
    if [ "$get_gpu_version" != "" ]; then
	for i in ${get_gpu_version}
    	do
        	if [ ! -f $Cur_Dir/$i-install.flag ]; then
	            package=`ls *$i*`
        	    file_type $package "$i"
	            cd $ins_path
        	    chmod 777 ./* -R
	            cd $ins_path/mods
        	    ./install_module.sh -i
	            if [ $? -eq 0 ]; then
        	        touch $Cur_Dir/$i-install.flag
                	echo "Install $i driver successful"
            	    else
                	echo "Install $i driver failed"
            #    exit 1
         	    fi
        	fi
	        cd $Cur_Dir
    	done
   fi
}

uninstall_driver()
{
    local i
    get_gpu_version=`lspci|grep -i -E "3D controller|NVIDIA Corporation" | sed 's/.*compatible controller:\(.*\)(.*/\1/' |uniq |sed 's/.*\(\[.*\]\).*/\1/' | awk '{print $2}' | sed 's/.$//'`
    if [ "$get_gpu_version" != "" ]; then
    	for i in ${get_gpu_version}
    	do
        	if [ ! -f $Cur_Dir/$i-install.flag ]; then
	            package=`ls *$i*`
        	    file_type $package "$i"
	            cd $ins_path
        	    chmod 777 ./* -R
	            cd $ins_path/mods
        	    ./install_module.sh -u
	            if [ $? -eq 0 ]; then
        	        echo "Uninstall $i driver successful"
	            else
        	        echo "Uninstall $i driver failed"
                	exit 1
	            fi
        	fi
	        cd $Cur_Dir
	done
   fi
}
get_gpu_version=`lspci|grep -i -E "3D controller|NVIDIA Corporation" | sed 's/.*compatible controller:\(.*\)(.*/\1/' |uniq |sed 's/.*\(\[.*\]\).*/\1/' | awk '{print $2}' | sed 's/.$//'`
if [ "$get_gpu_version" != "" ]; then
	sh $Cur_Dir/NVIDIA-Linux-x86_64-384.111.run -s
	if [ $? -ne 0 ]; then
   
	   rmmod -f mods 
	   uninstall_driver
	   rmmod -f nouveau

	   sh NVIDIA-Linux-x86_64-384.111.run -s --uninstall
	   sh NVIDIA-Linux-x86_64-384.111.run -s -a
	   if [ $? -ne 0 ]; then
		echo "nvidia-smi driver install fail"
		error_flag=1	
	   fi
	fi
	cd $Cur_Dir/
	cp -f $Cur_Dir/blacklist.conf /etc/modprobe.d/
	mv -f /boot/initramfs-$(uname -r).img /boot/initramfs-$(uname -r).img.bak
	dracut -v /boot/initramfs-$(uname -r).img $(uname -r)
fi

