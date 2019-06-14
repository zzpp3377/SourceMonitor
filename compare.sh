#!/bin/sh

iperf -c 10.18.134.8 -p 20000 -i 1  -t 11 -u -b 0.1M > iperf.log &
#iperf -c 10.18.133.8 -p 20000 -i 1 -t 11 -w 4K > iperf.log &

str=$(ps -ef | grep iperf )
pid=$(echo $str | awk 'NR==1{ print $2 }')
echo $pid
nethogs -d 1 -c 10 -t >nethogs.log &
./test 10 1 ${pid} >test.log &

