#!/bin/bash

if [ $# -ne 1 ]; then
	echo "Usage: ./run_memeater.sh SZGB"
	exit
fi

./clearcache.sh

numactl -N 0 -m 1 sudo ./memeater $1

