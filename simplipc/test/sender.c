#include <stdio.h>
#include <stdlib.h>
#include <simpl.h>
#include <sys/time.h>

#define LIMIT	500

unsigned maxSize = 100 * LIMIT;

int main(int argc, char **argv)
{
int receiver;
char mem[maxSize];
int ret;
int i;
struct timeval start;
struct timeval stop;
time_t total;
unsigned size = 1024;

if (name_attach(argv[0], NULL) == -1)
	{
	printf("%s: cannot attach name: %s\n", argv[0], whatsMyError());
	exit(-1);
	}

receiver = name_locate(argv[1]);
if (receiver == -1)
	{
	printf("%s: cannot locate receiver: %s\n", argv[0], whatsMyError());
	exit(-1);
	}

// get start time
gettimeofday(&start, NULL);

for (i = 0; i < LIMIT; i++)
	{
	//printf("sending message %d\n", i+1);

	//size += 100;

	//ret = Send(receiver, mem, mem, size, maxSize);
	//ret = Send(receiver, mem, NULL, size, maxSize);
	ret = Send(receiver, mem, mem, size, size);
	if (ret == -1)
		{
		printf("%s: Send error: %s\n", argv[0], whatsMyError());
		exit(-1);
		}

	//printf("ret=%d\n", ret); 
	//simplScopy(mem, ret);
	}

// get stop time
gettimeofday(&stop, NULL);

total = (stop.tv_sec - start.tv_sec) * 1000 + (stop.tv_usec - start.tv_usec) / 1000;
printf("time taken=%lu ms\n", total);
printf("round trips=%d\n", LIMIT);
printf("message size=1024 bytes\n");

return(0);
}
