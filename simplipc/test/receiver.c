#include <stdio.h>
#include <stdlib.h>
#include <simpl.h>

#define LIMIT	500

unsigned maxSize = 100 * LIMIT;  

int main(int argc, char **argv)
{
char *sender;
char mem[maxSize];
int n;

if (name_attach(argv[0], NULL) == -1)
	{
	printf("receiver: cannot attach name: %s\n", whatsMyError());
	exit(-1);
	}

/*
use this sleep at start fro "ps ax", get pid and "kill -s SIGKILL pid"
the run the sender and check that it locates the orphaned receiver fifo
*/
//sleep(60);

while (1)
	{
	//= Receive(&sender, NULL, 0);
	n = Receive(&sender, mem, 1024);
	if (n == -1)
		{
		printf("receiver: Receive error: %s\n", whatsMyError());
		continue;
		}

	//printf("received message %d\n", ++i);
	//printf("simpl reply size = %d\n", simplReplySize(sender));

	if (Reply(sender, mem, 1024) == -1)
	//simplRcopy(sender, mem, n);
	//if (Reply(sender, mem, n) == -1)
		{
		printf("receiver: Reply error: %s\n", whatsMyError());
		continue;
		}
	}

return(0);
}
