#include "pebs.h"
#include <sys/resource.h>

int main(int argc, char **argv) {
	init();
	
	assert(0 == pthread_create(&sample_thread_t, NULL, sample_thread_func, NULL));
	assert(0 == pthread_create(&sample_fast_t, NULL, sample_fast_func, NULL));
	assert(0 == pthread_create(&sample_slow_t, NULL, sample_slow_func, NULL));
	
	void *thread_ret;
	assert(0 == pthread_join(sample_thread_t, &thread_ret));
	assert(PTHREAD_CANCELED == thread_ret);
	
	
	void *thread_ret_fast;
	assert(0 == pthread_join(sample_fast_t, &thread_ret_fast));
	assert(PTHREAD_CANCELED == thread_ret_fast);
	
	
	void *thread_ret_slow;
	assert(0 == pthread_join(sample_slow_t, &thread_ret_slow));
	assert(PTHREAD_CANCELED == thread_ret_slow);
	
	
	
	
	
	

	return 0;
}
