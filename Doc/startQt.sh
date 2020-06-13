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

while [ 1 ] ; do
	find /opt/log -type f -mtime +15 -exec rm -f {} \;
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
