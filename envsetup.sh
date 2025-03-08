#!/bin/bash

# ref
# https://github.com/vtess/Pond/blob/master/cxl-global.sh

flush_fs_caches()
{
	echo 3 | sudo tee /proc/sys/vm/drop_caches >/dev/null 2>&1
	sleep 5
}

disable_nmi_watchdog()
{
	echo 0 | sudo tee /proc/sys/kernel/nmi_watchdog >/dev/null 2>&1
}

disable_turbo()
{
	echo 1 | sudo tee /sys/devices/system/cpu/intel_pstate/no_turbo >/dev/null 2>&1
}

enable_turbo()
{
	echo 0 | sudo tee /sys/devices/system/cpu/intel_pstate/no_turbo >/dev/null 2>&1
}

# 0: no randomization, everything is static
# 1: conservative randomization, shared libraries, stack, mmap(), VDSO and heap are randomized
# 2: full randomization, the above points to 1 plus brk()
disable_va_aslr()
{
	echo 0 | sudo tee /proc/sys/kernel/randomize_va_space >/dev/null 2>&1
}

disable_swap()
{
	sudo swapoff -a
}

disable_ksm()
{
	echo 0 | sudo tee /sys/kernel/mm/ksm/run >/dev/null 2>&1
}

disable_numa_balancing()
{
	echo 0 | sudo tee /proc/sys/kernel/numa_balancing >/dev/null 2>&1
}

disable_thp()
{
	echo never | sudo tee /sys/kernel/mm/transparent_hugepage/enabled >/dev/null 2>&1
}

disable_ht()
{
	echo off | sudo tee /sys/devices/system/cpu/smt/control >/dev/null 2>&1
}

enable_ht()
{
	echo on | sudo tee /sys/devices/system/cpu/smt/control >/dev/null 2>&1
}

set_performance_mode()
{
	for governor in /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor; do
		echo performance | sudo tee $governor >/dev/null 2>&1
	done
}

set_powersave_mode()
{
	for governor in /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor; do
		echo powersave | sudo tee $governor >/dev/null 2>&1
	done
}

check_pmqos()
{
	local pmqospid=$(ps -ef | grep pmqos | grep -Ev 'sudo |grep' | awk '{print $2}')
	#set_performance_mode
	[[ -n "$pmqospid" ]] && return

	sudo nohup ./pmqos >/dev/null 2>&1 &
	sleep 3

	# double check
	pmqospid=$(ps -ef | grep pmqos | grep -Ev 'sudo |grep' | awk '{print $2}')
	if [[ -z "$pmqospid" ]]; then
		echo "Error: failed to start pmqos!"
		exit
	fi
}


sudo service mysql stop

set_performance_mode
disable_nmi_watchdog
disable_va_aslr
disable_ksm
disable_numa_balancing
disable_thp
disable_ht
disable_turbo
check_pmqos
disable_swap

flush_fs_caches

