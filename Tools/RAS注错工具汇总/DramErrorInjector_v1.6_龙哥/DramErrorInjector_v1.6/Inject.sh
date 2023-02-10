#/bin/sh

for((i=0;i<$1;i++))
do
./DramErrorInjector_v1.6 -A 2 -a $2 -t 1 -m 0 -v 1 -p 0
currentloop=`expr $i + 1`
echo -e "\033[31m===========================\r\nCurrent Loop:"$currentloop"\r\n===========================\r\n\033[0m"
done
