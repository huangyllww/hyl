#!/bin/sh
dir=$PWD
echo Installing HPL
tar -xf AMD_HPL.tar.gz
tar -xf HPL.tar.gz
sudo rm -rf HPL.tar.gz
tar -xf openmpi.tar.gz
sudo mv openmpi /opt/openmpi
sudo rm -rf openmpi*
tar -xf blis.tar.gz
sudo mv blis /opt/blis
sudo rm -rf blis*
tar -xf knem.tar.gz
cd knem-1.1.2
sudo ./configure --prefix=/opt/knem
sudo make clean
sudo make install
sudo /opt/knem/sbin/knem_local_install
sudo modprobe knem
cd $dir
sudo rm -rf knem*
echo HPL setup ready for run
