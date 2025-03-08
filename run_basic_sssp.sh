#!/bin/bash
./envsetup.sh

DIRNAME="sssp_data/fast"
mkdir -p ${DIRNAME}
for i in {1..3}; do
	./clearcache.sh
	OMP_NUM_THREADS=4 GOMP_CPU_AFFINITY="24-27" numactl -m 1 ./sssp -f benchmark/graphs/twitter.wsg -n16 -d2 >${DIRNAME}/${i}
done

DIRNAME="sssp_data/slow"
mkdir -p ${DIRNAME}
for i in {1..3}; do
	./clearcache.sh
	OMP_NUM_THREADS=4 GOMP_CPU_AFFINITY="24-27" numactl -m 3 ./sssp -f benchmark/graphs/twitter.wsg -n16 -d2 >${DIRNAME}/${i}
done

for mem in {78..24..2}; do
	./clearcache.sh
	nohup ./run_memeater.sh ${mem} 1>>nohup.memeaterout 2>>nohup.memeatererr &
	sleep 70
	DIRNAME="sssp_data/m${mem}"
	mkdir -p ${DIRNAME}

	for i in {1..3};do
		./clearcache.sh
		OMP_NUM_THREADS=4 GOMP_CPU_AFFINITY="24-27" numactl -m 1,3 ./sssp -f benchmark/graphs/twitter.wsg -n16 -d2 >${DIRNAME}/${i}
		sleep 3
	done

	sudo pkill memeater
	sleep 30
done
