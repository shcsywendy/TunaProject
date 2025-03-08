#!/bin/bash
DIRNAME="test_bfs"
mkdir -p ${DIRNAME}

./clearcache.sh; OMP_NUM_THREADS=4 GOMP_CPU_AFFINITY="24-27" ./bfs -f benchmark/graphs/twitter.sg -n256 >${DIRNAME}/1

#./clearcache.sh;OMP_NUM_THREADS=4 GOMP_CPU_AFFINITY="24-27" ./bfs -f benchmark/graphs/twitter.sg -n256 >${DIRNAME}/2

#./clearcache.sh;OMP_NUM_THREADS=4 GOMP_CPU_AFFINITY="24-27" ./bfs -f benchmark/graphs/twitter.sg -n256 >${DIRNAME}/3

