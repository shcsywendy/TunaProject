#!/bin/bash

if [ $# -ne 1 ]; then
	echo "Usage: ./run_memeater.sh SZGB"
	exit
fi

./clearcache.sh

numactl -C 20 -m 1 sudo ./memeater $1

