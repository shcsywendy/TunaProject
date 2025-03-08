#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include <assert.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	if (argc != 2) {
		fprintf(stderr, "Usage: ./memeater SZGB\n");
		return 1;
	}
	uint64_t SZGB = atoi(argv[1]);
	assert(SZGB>0);
	uint64_t bufsize = SZGB*1024*1024*1024ULL;
	printf("size=%lu\n", bufsize);

	char *buf = calloc(1, bufsize);
	assert(buf);

	assert(0 == mlock((void *)buf, bufsize));

	printf("%luGB is locked\n", SZGB);
	//while (1);
	sleep(40000);

	return 0;

}




