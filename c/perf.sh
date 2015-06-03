#!/bin/bash

# Set all CPUs to default frequency
#for i in 0 1 2 3 4 5 6 7; do
#	echo ondemand > /sys/devices/system/cpu/cpu$i/cpufreq/scaling_governor
#done
#
#sleep 1
#
#echo "CPU Frequency with 'ondemand' governor:"
#echo ""
#cat /proc/cpuinfo | grep MHz
#echo ""
#
## Run test
#perf stat ./sha1

# Set all CPUs to maximum frequency
for i in 0 1 2 3 4 5 6 7; do
	echo performance > /sys/devices/system/cpu/cpu$i/cpufreq/scaling_governor
done

sleep 1

echo "CPU Frequency with 'performance' governor:"
echo ""
cat /proc/cpuinfo | grep MHz
echo ""

make clean sha1_threads

sudo /home/amit/src/linux-2.6/tools/perf/perf record -s ./sha1_threads
sudo /home/amit/src/linux-2.6/tools/perf/perf report -T

# Run test
#perf record ./sha1
#perf report --stdio
#perf annotate --stdio sha1_block_data_order_avx

# Run timechart
#perf timechart record ./sha1
#perf timechart -p sha1
#inkscape output.svg
