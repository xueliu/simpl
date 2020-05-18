// this program works with sender1.c

#include <stdio.h>
#include <stdlib.h>
#include <simpl.h>

int main()
{
char *sender;
char mem[1024];
int n;
int i = 0;

if (name_attach("receiver", NULL) == -1)
	{
	printf("receiver: cannot attach name: %s\n", whatsMyError());
	exit(-1);
	}

while (1)
	{
	n = Receive(&sender, mem, 1024);
	if (n == -1)
		{
		printf("receiver: Receive error: %s\n", whatsMyError());
		continue;
		}

	printf("received message %d\n", ++i);

	n = Reply(sender, mem, 1024);
	if (n == -1)
		{
		printf("receiver: Reply error: %s\n", whatsMyError());
		exit(-1);
		}
			
	printf("receiver: reply bytes=%d\n", n);
	}

return(0);
}
