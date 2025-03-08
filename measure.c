#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
	system("nohup numactl --cpunodebind=0 --membind=0 sudo ./pebs &");

	sleep(30);

	system("sudo pkill pebs");

	return 0;

}



