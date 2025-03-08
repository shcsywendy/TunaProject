#ifndef PEBS_H
#define PEBS_H

#include <stdio.h>
#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>
#include <assert.h>
#include <sys/time.h>
#include <unistd.h>
#include <asm/unistd.h>
#include <linux/perf_event.h>
#include <linux/hw_breakpoint.h>
#include <sys/mman.h>
#include <sched.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <cstring>
#include <queue>
#include <map>
#include <set>
#include <perfmon/pfmlib.h>
#include <perfmon/pfmlib_perf_event.h>
#include <err.h>
#include <signal.h>
#include <cerrno>
#include <cstddef>
#include <cstdio>

//#define PERF_PAGES (1 + (1<<18))
//#define PERF_PAGES (1 + (1<<12))
#define PERF_PAGES (1 + (1<<6))
//#define SAMPLE_PERIOD 100000
#define SAMPLE_PERIOD 500000
#define CPUNUM 4
#define SAMPLE_CPUID 23
#define SAMPLE_FAST_CPUID 21
#define SAMPLE_SLOW_CPUID 22

// typedef unsigned long long __u64;

struct perf_sample {
	struct perf_event_header header;
	__u64 ip; /* if PERF_SAMPLE_IP */
	__u32 pid, tid; /* if PERF_SAMPLE_TID */
	__u64 time; /* if PERF_SAMPLE_TIME */
	__u64 addr; /* if PERF_SAMPLE_ADDR */
	//__u64 phys_addr; /* if PERF_SAMPLE_PHYS_ADDR */
};

enum pbuftype {
	FAST=0,
	SLOW=1,
	NPBUFTYPES
};

extern unsigned long long fast_counter;
extern unsigned long long slow_counter;

extern int cpuids[CPUNUM];

extern void *sample_thread_func(void*);

//add for fast and slow
extern void *sample_fast_func(void*);
extern void *sample_slow_func(void*);


extern pthread_t sample_thread_t;

//add for fast and slow
extern pthread_t sample_fast_t;
extern pthread_t sample_slow_t;


extern struct perf_event_mmap_page *perf_page[CPUNUM][NPBUFTYPES];
extern int pfd[CPUNUM][NPBUFTYPES];

extern char fast_filename[64];
extern FILE *fastfp;
extern char slow_filename[64];
extern FILE *slowfp;

extern struct perf_event_attr attrs[CPUNUM][NPBUFTYPES];
extern struct perf_event_mmap_page *_get_perf_page(int pfd);
extern void init();
extern void perf_setup();

extern void INThandler(int);
extern long _perf_event_open(struct perf_event_attr *hw_event, pid_t pid, int cpu, int group_fd, unsigned long flags);
extern __u64 _get_read_attr();
extern __u64 _get_write_attr();
//local dram
extern __u64 _get_local_dram_read_attr();
//remote dram
extern __u64 _get_remote_dram_read_attr();
//local pm
extern __u64 _get_local_PM_read_attr();
//remote pm
extern __u64 _get_remote_PM_read_attr();
#endif
