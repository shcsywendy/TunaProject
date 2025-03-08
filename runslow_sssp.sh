#!/bin/bash
DIRNAME="slow_bind3"
mkdir -p ${DIRNAME}

./clearcache.sh; export OMP_NUM_THREADS=4; export GOMP_CPU_AFFINITY="24-27"; numactl -m 3 ./sssp -f benchmark/graphs/twitter.wsg -n16 -d2 >${DIRNAME}/1

./clearcache.sh; export OMP_NUM_THREADS=4; export GOMP_CPU_AFFINITY="24-27"; numactl -m 3 ./sssp -f benchmark/graphs/twitter.wsg -n16 -d2 >${DIRNAME}/2

./clearcache.sh; export OMP_NUM_THREADS=4; export GOMP_CPU_AFFINITY="24-27"; numactl -m 3  ./sssp -f benchmark/graphs/twitter.wsg -n16 -d2 >${DIRNAME}/3

