# TunaProject

Tiered memory, built upon a combination of fast memory and slow memory, provides a cost-effective solution to meet ever-increasing requirements from emerging applications for large memory capacity. Reducing the size of fast memory is valuable to improve memory utilization in production and reduce production costs because fast memory tends to be expensive. However, deciding the fast memory size is challenging because there is a complex interplay between application characterization and the overhead of page migration used to mitigate the impact of limited fast memory capacity. In this paper, we introduce a system, Tuna, to decide fast memory size based on modeling of page migration. Tuna uses micro-benchmarking to model the impact of page migration on application performance using three metrics. Tuna decides the fast memory size based on offline modeling results and limited information on workload telemetry. Evaluating with common big-memory applications and using 5% as the performance loss target, we show that Tuna in combination with a page management system (TPP) saves fast memory by 8.5% on average (up to 16%). This is in contrast to the 5% saving in fast memory reported by Microsoft Pond for the same workloads (BFS and SSSP) and the same performance loss target.

## Setup
### Run the setup script to configure the environment:
./envsetup.sh

### Running Benchmarks
To start the benchmarks using the script:

bash run_basic_sssp.sh  # Automatically runs and stores results in sssp_data/fast, sssp_data/slow, and sssp_data/m{memory_amount}
### Details
Fast and Slow Configurations: Benchmarks run on NUMA nodes 1 and 3.
Memory Pressure: Simulates environments with decreasing memory from 78GB to 24GB

## arxiv link
https://arxiv.org/abs/2410.00328
