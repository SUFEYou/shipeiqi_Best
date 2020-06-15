#!/bin/bash

checkprocess()
{
 if [ "$1" = "" ];
   then
      return 1
 fi
process_num=`ps | grep "$1" | grep -v "grep" | wc -l`
if [ $process_num -eq 1 ];
   then
      return 0
   else
      return 1
fi

}

checkFileCount()
{
check=$(ls $1)
count=0
for item in $check
do
    count=$[ $count + 1 ]
done
return $count
}

logFileCountCheck()
{
checkFileCount $1
check_result=$?
if [ $check_result -gt $2 ]; then
    delNum=$[ $check_result - $2 ]
    loglist=$(ls -t /opt/log/ | tail -$delNum)
    for log in $loglist
    do
        rm -f /opt/log/$log
    done
fi
}

while [ 1 ] ; do
    #根据现场使用情况，每次使用系统间隔可能大于15天，不能使用时间判断，目前采用保留最新的30条日志
	#find /opt/log -type f -mtime +15 -exec rm -f {} \;
	logFileCountCheck "/opt/log/" "30"
	checkprocess "Adapter_VHF"
	check_result0=$?
	checkprocess "VHF_Box"
	check_result1=$?
	if [ $check_result0 -eq 1 ] && [ $check_result1 -eq 1 ]; then
		/opt/killQt.sh
		if [ -e Adapter_VHF ]; then
			chmod 777 /opt/Adapter_VHF
			/opt/Adapter_VHF &
		elif [ -e VHF_Box ]; then
			chmod 777 /opt/VHF_Box
			/opt/VHF_Box -qws -nomouse &
		fi
	fi
	sleep 10
done
