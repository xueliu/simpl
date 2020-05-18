#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <simpl.h>

#define LIMIT		 1000
#define TCP_PROXY	5
#define TCP_X_PROXY	999

int getRandomNum(int, int);

int main(int argc, char **argv)
{
const char *me = "remoteSender";
int receiverTCP;
int receiverTCP_X;
char mem[1024];
int ret;
int i;
int sleepTime;
int outSize;

if (name_attach(argv[1], NULL) == -1)
	{
	printf("%s: cannot attach name: %s-%s\n", me, argv[1], whatsMyError());
	exit(-1);
	}

receiverTCP = name_locate(argv[2]);
if (receiverTCP == -1)
	{
	printf("%s: cannot locate receiver: %s-%s\n", me, argv[2], whatsMyError());
	exit(-1);
	}

receiverTCP_X = name_locate(argv[3]);
if (receiverTCP_X == -1)
	{
	printf("%s: cannot locate receiver: %s-%s\n", me, argv[3], whatsMyError());
	exit(-1);
	}

for (i = 1; i <= LIMIT; i++)
	{
	// get a random number for the sleep
	sleepTime = getRandomNum(0, 60);
	if (sleepTime == -1)
		{
		printf("%s: invalid sleep time\n", me);
		exit(-1);
		}
		
	// get a random number for the size of the message to be sent
	outSize = getRandomNum(1, 1024);
	if (outSize == -1)
		{
		printf("%s: invalid outgoing message size\n", me);
		exit(-1);
		}
	
	// sleep a random amount of time before next send
	//sleep(sleepTime);
	sleep(10);
	
	// decide what to send and by what protocol via outSize and sleepTime
	if (sleepTime % 2)
		{
		// outSize is an odd number, do a TCP Send
		printf("%s: Sending TCP message\n", me);
		ret = Send(receiverTCP, mem, mem, outSize, 1024);
		if (ret == -1)
			{
			printf("%s: Send error to %s-%s\n", me, argv[2], whatsMyError());
			exit(-1);
			}
		}
	else
		{
		// sleepTime is an even number, send something via TCP_X
		if (outSize % 2)
			{
			// outSize is an odd number, do a Send
			printf("%s: Sending TCP_X message\n", me);
			ret = Send(receiverTCP_X, mem, mem, outSize, 1024);
			if (ret == -1)
				{
				printf("%s: Send error to %s-%s\n", me, argv[3], whatsMyError());
				exit(-1);
				}
			}
		else
			{
			// outSize is an even number, do a Trigger
			printf("%s: Triggering TCP_X message\n", me);
			ret = Trigger(receiverTCP_X, TCP_X_PROXY);
			if (ret == -1)
				{
				printf("%s: Trigger error to %s-%s\n", me, argv[3], whatsMyError());
				exit(-1);
				}
			}
		}
	}

return(0);
}

/******************************************************************************/

int getRandomNum(int low, int high)
{
int limit;
int ran;
int num;

#define MAX 0x7FFFFFFF

if (high < 0)
	{
	return(-1);
	}

if (low < 0)
	{
	return(-1);
	}

limit = high - low + 1;

if ( limit < 2)
	{
	return(-1);
	}

ran = rand();

num = (int) (((float)ran / (float)MAX) * (high - low + 1) + low);

return(num);
}
