#define _GNU_SOURCE
#include "pebs.h"

// DEFINITION
int cpuids[CPUNUM] = {24, 25, 26, 27};
struct perf_event_attr attrs[CPUNUM][NPBUFTYPES];
struct perf_event_mmap_page *perf_page[CPUNUM][NPBUFTYPES];
int pfd[CPUNUM][NPBUFTYPES];

unsigned long long fast_counter = 0;
unsigned long long slow_counter = 0;

char fast_filename[64];
FILE *fastfp;
char slow_filename[64];
FILE *slowfp;
pthread_t sample_thread_t;
// added for sample_fast and sample_slow
pthread_t sample_fast_t;
pthread_t sample_slow_t;

void init()
{
	__u64  ts = time(NULL);
	snprintf(fast_filename, sizeof(fast_filename), "fast_%lu", ts);
	fastfp = fopen(fast_filename, "w");
	if (!fastfp) {
		fprintf(stderr, "fopen file[%s] error!\n", fast_filename);
		assert(fastfp != NULL);
	}
	
	snprintf(slow_filename, sizeof(slow_filename), "slow_%lu", ts);
	slowfp = fopen(slow_filename, "w");
	if (!slowfp) {
		fprintf(stderr, "fopen file[%s] error!\n", slow_filename);
		assert(slowfp != NULL);
	}

	for (int i=0; i<CPUNUM; ++i) {
		for (int j=0; j<NPBUFTYPES; ++j) {
			perf_page[i][j] = NULL;
			pfd[i][j] = -1;
		}
	}

	int ret = pfm_initialize();
	if (ret != PFM_SUCCESS) {
		fprintf(stderr, "Cannot initialize library: %s\n", pfm_strerror(ret));
		assert(ret == PFM_SUCCESS);
	}
	perf_setup();
}

void perf_setup()
{
	for (int i=0; i<CPUNUM; ++i) {
		for (int j=0; j<NPBUFTYPES; j++){
			assert(j==FAST || j==SLOW);
			memset(&attrs[i][j], 0, sizeof(struct perf_event_attr));
			attrs[i][j].type = PERF_TYPE_RAW;
			attrs[i][j].size = sizeof(struct perf_event_attr);

			if (j == FAST) attrs[i][j].config = _get_local_dram_read_attr();
			else if (j == SLOW) attrs[i][j].config = _get_local_PM_read_attr();

			attrs[i][j].config1 = 0;
			attrs[i][j].sample_period = SAMPLE_PERIOD;
			attrs[i][j].sample_type = PERF_SAMPLE_IP | PERF_SAMPLE_TID| PERF_SAMPLE_ADDR | PERF_SAMPLE_TIME;
			attrs[i][j].disabled = 0;
			attrs[i][j].exclude_kernel = 1;
			attrs[i][j].exclude_hv = 1;
			attrs[i][j].exclude_idle = 1;
			attrs[i][j].exclude_callchain_kernel = 1;
			attrs[i][j].exclude_callchain_user = 1;
			attrs[i][j].precise_ip = 1;

			pfd[i][j] = _perf_event_open(&attrs[i][j], -1/*pid*/, cpuids[i], -1, 0);
			if (pfd[i][j] == -1) {
				if (errno == ESRCH) fprintf(stderr, "No such process(-1)\n");
				assert(pfd[i][j] != -1);
			}
			perf_page[i][j] = _get_perf_page(pfd[i][j]);
			assert(perf_page[i][j] != NULL);
		} // end of FAST and SLOW
	} // end of setup events for each CPU
	signal(SIGINT, INThandler);
}

void *sample_fast_func(void *arg)
{
	assert(0 == pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL));
	assert(0 == pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL));
	// set affinity
	cpu_set_t cpuset;
	CPU_ZERO(&cpuset);
	CPU_SET(SAMPLE_FAST_CPUID, &cpuset);
	pthread_t thread = pthread_self();
	assert(0 == pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset));
	
	// check if run into sample fast or not
	fprintf(fastfp,"=====================fast func begin==============================\n");

	while (true) {
		for (int i=0; i<CPUNUM; ++i) {
			struct perf_event_mmap_page *p = perf_page[i][FAST];
			char *pbuf = (char *)p + p->data_offset;
			__sync_synchronize();
			if (p->data_head == p->data_tail)	continue;
			struct perf_event_header *ph =
				reinterpret_cast<struct perf_event_header*>(pbuf + (p->data_tail % p->data_size));
			assert(ph != NULL);
			struct perf_sample *ps;
			switch (ph->type) {
				case PERF_RECORD_SAMPLE:
					ps = (struct perf_sample*)ph;
					assert(ps != NULL);
					if (ps->addr && ps->time)	
					{
						fprintf(fastfp,"case PERF_RECORD_SAMPLE fast\n");
						fprintf(fastfp, "%llu %llu\n", ps->addr>>12, ps->time);
					}
					else	printf("fast ZERO PAGE\n");
					break;
				case PERF_RECORD_THROTTLE:
					printf("fast PERF_RECORD_THROTTL\n");
					break;
				case PERF_RECORD_UNTHROTTLE: 
					printf("fast PERF_RECORD_UNTHROTTLE\n");
					break;
				default: 
					printf("fast Unknown perf_sample type\n");
					break;
			} // retrive the sample record, go next
			p->data_tail += ph->size;
		} // iteration on each target CPU
	} // Repeated Sampling
	return NULL;
}

void *sample_slow_func(void *arg)
{
	assert(0 == pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL));
	assert(0 == pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL));
	// set affinity
	cpu_set_t cpuset;
	CPU_ZERO(&cpuset);
	CPU_SET(SAMPLE_SLOW_CPUID, &cpuset);
	pthread_t thread = pthread_self();
	assert(0 == pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset));
	
	// check if run into sample slow or not
	fprintf(slowfp,"=================================slow func begin==================================\n");
	
	while (true) {
		for (int i=0; i<CPUNUM; ++i) {
			struct perf_event_mmap_page *p = perf_page[i][SLOW];
			char *pbuf = (char *)p + p->data_offset;
			__sync_synchronize();
			if (p->data_head == p->data_tail)	continue;
			struct perf_event_header *ph =
				reinterpret_cast<struct perf_event_header*>(pbuf + (p->data_tail % p->data_size));
			assert(ph != NULL);
			struct perf_sample *ps;
			switch (ph->type) {
				case PERF_RECORD_SAMPLE:
					ps = (struct perf_sample*)ph;
					assert(ps != NULL);
					if (ps->addr && ps->time)	
					{
						fprintf(slowfp,"case PERF_RECORD_SAMPLE slow\n");
						fprintf(slowfp, "%llu %llu\n", ps->addr>>12, ps->time);
					}
					else	printf("slow ZERO PAGE\n");
					break;
				case PERF_RECORD_THROTTLE:
					printf("slow PERF_RECORD_THROTTL\n");
					break;
				case PERF_RECORD_UNTHROTTLE: 
					printf("slow PERF_RECORD_UNTHROTTLE\n");
					break;
				default: 
					printf("slow Unknown perf_sample type\n");
					break;
			} // retrive the sample record, then go next
			p->data_tail += ph->size;
		} // iteration on each target CPU
	} // Repeated Sampling
	return NULL;
}

void *sample_thread_func(void *arg)
{
	assert(0 == pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL));
	assert(0 == pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL));

	// set affinity
	cpu_set_t cpuset;
	CPU_ZERO(&cpuset);
	CPU_SET(SAMPLE_CPUID, &cpuset);
	pthread_t thread = pthread_self();
	assert(0 == pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset));

	while (true) {
		for (int i=0; i<CPUNUM; ++i) {
			for (int j=0; j<NPBUFTYPES; ++j) {
				struct perf_event_mmap_page *p = perf_page[i][j];
				char *pbuf = (char *)p + p->data_offset;
				__sync_synchronize();
				if (p->data_head == p->data_tail)	continue;
				struct perf_event_header *ph = 
					reinterpret_cast<struct perf_event_header*>(pbuf + (p->data_tail % p->data_size));
				assert(ph != NULL);
				struct perf_sample *ps;
				switch (ph->type) {
					case PERF_RECORD_SAMPLE:
						ps = (struct perf_sample*)ph; assert(ps != NULL);
						if (ps->addr && ps->time) {
							if (j == FAST)	++fast_counter;
							else if (j == SLOW)	++slow_counter;
							else printf("buftype=%d Exception!\n", j);
						}else
							printf("buftype=%d ZERO PAGE\n", j);
						break;
					case PERF_RECORD_THROTTLE:
						printf("buftype=%d PERF_RECORD_THROTTL\n", j);
						break;
					case PERF_RECORD_UNTHROTTLE: 
						printf("buftype=%d PERF_RECORD_UNTHROTTLE\n", j);
						break;
					default: 
						printf("buftype=%d Unknown perf_sample type\n", j);
						break;
				} // got the perf sample
				p->data_tail += ph->size;
			} // end of loop NPBUFTYPES
		} // end of loop for each target CPU
	} // Repeated Sampling
	return NULL;
}

long _perf_event_open(struct perf_event_attr *hw_event, pid_t pid, int cpu, int group_fd, unsigned long flags)
{
	int ret = syscall(__NR_perf_event_open, hw_event, pid, cpu, group_fd, flags);
	return ret;
}

__u64 _get_read_attr()
{
	struct perf_event_attr attr;
	memset(&attr, 0, sizeof(attr));
	int ret = pfm_get_perf_event_encoding("MEM_LOAD_L3_MISS_RETIRED.LOCAL_DRAM", 
										PFM_PLMH, &attr, NULL, NULL);
	if (ret != PFM_SUCCESS) {
		fprintf(stderr, "Cannot get encoding %s\n", pfm_strerror(ret));
		assert(ret == PFM_SUCCESS);
	}
	return attr.config;
}

__u64 _get_write_attr()
{
	struct perf_event_attr attr;
	memset(&attr, 0, sizeof(attr));
	int ret = pfm_get_perf_event_encoding("MEM_INST_RETIRED.STLB_MISS_STORES",
										PFM_PLMH, &attr, NULL, NULL);
	if (ret != PFM_SUCCESS) {
		fprintf(stderr, "Cannot get encoding %s\n", pfm_strerror(ret));
		assert(ret == PFM_SUCCESS);
	}
	return attr.config;
}

__u64 _get_local_dram_read_attr(){
	struct perf_event_attr attr;
	memset(&attr, 0, sizeof(attr));
	// here we will change the evert name 
	int ret = pfm_get_perf_event_encoding("MEM_LOAD_L3_MISS_RETIRED.LOCAL_DRAM",
										PFM_PLMH, &attr, NULL, NULL);
	if (ret != PFM_SUCCESS) {
		fprintf(stderr, "Cannot get encoding %s\n", pfm_strerror(ret));
		assert(ret == PFM_SUCCESS);
	}
	return attr.config;
}

__u64 _get_remote_dram_read_attr(){
	struct perf_event_attr attr;
	memset(&attr, 0, sizeof(attr));
	// here we will change the evert name 
	int ret = pfm_get_perf_event_encoding("MEM_LOAD_L3_MISS_RETIRED.REMOTE_DRAM",
										PFM_PLMH, &attr, NULL, NULL);
	if (ret != PFM_SUCCESS) {
		fprintf(stderr, "Cannot get encoding %s\n", pfm_strerror(ret));
		assert(ret == PFM_SUCCESS);
	}
	return attr.config;
}

__u64 _get_local_PM_read_attr(){
	struct perf_event_attr attr;
	memset(&attr, 0, sizeof(attr));
	// here we will change the evert name 
	int ret = pfm_get_perf_event_encoding("MEM_LOAD_RETIRED.LOCAL_PMM",
										PFM_PLMH, &attr, NULL, NULL);
	if (ret != PFM_SUCCESS) {
		fprintf(stderr, "Cannot get encoding %s\n", pfm_strerror(ret));
		assert(ret == PFM_SUCCESS);
	}
	return attr.config;
}

__u64 _get_remote_PM_read_attr(){
	struct perf_event_attr attr;
	memset(&attr, 0, sizeof(attr));
	// here we will change the evert name 
	int ret = pfm_get_perf_event_encoding("MEM_LOAD_L3_MISS_RETIRED.REMOTE_PMM",
										PFM_PLMH, &attr, NULL, NULL);
	if (ret != PFM_SUCCESS) {
		fprintf(stderr, "Cannot get encoding %s\n", pfm_strerror(ret));
		assert(ret == PFM_SUCCESS);
	}
	return attr.config;
}

struct perf_event_mmap_page *_get_perf_page(int pfd)
{
	// for this config; they map 4KB * PERF_PAGES. ()
	size_t mmap_size = sysconf(_SC_PAGESIZE) * PERF_PAGES;
	// printf("mmap_size %ld\n", mmap_size);
	struct perf_event_mmap_page *p =
		reinterpret_cast<struct perf_event_mmap_page *>(mmap(NULL, mmap_size, PROT_READ | PROT_WRITE, 
																MAP_SHARED  , pfd, 0));

	if (p == MAP_FAILED) {
		fprintf(stderr, "mmap for pfd(%d) failed!\n", pfd);
		assert(p != MAP_FAILED);
	}
	return p;
}

void INThandler(int sig)
{
	signal(sig, SIG_IGN);
	assert(0 == pthread_cancel(sample_thread_t));
	
	//added for fast and slow
	assert(0 == pthread_cancel(sample_fast_t));
	assert(0 == pthread_cancel(sample_slow_t));
	
	for (int i=0; i<CPUNUM; ++i) {
		for (int j=0; j<NPBUFTYPES; ++j) {
			if (perf_page[i][j]) {
				munmap(perf_page[i][j], sysconf(_SC_PAGESIZE)*PERF_PAGES);
				perf_page[i][j] = NULL;
			}
			if (pfd[i][j] != -1) {
				ioctl(pfd[i][j], PERF_EVENT_IOC_DISABLE, 0);
				close(pfd[i][j]);
				pfd[i][j] = -1;
			}
		}
	}
	
	
	fprintf(fastfp,"print for fast_counter\n");
	fprintf(fastfp, "%llu\n", fast_counter);
	
	fprintf(slowfp,"print for slow_counter\n");
	fprintf(slowfp, "%llu\n", slow_counter);

	fclose(fastfp);
	fastfp = NULL;
	fclose(slowfp);
	slowfp = NULL;
}

