/*==========================================
 	This program works with sender2.c

 run as

 receiver simplname options

 where options
 	delay - delay the reply
 	normal - reply right away
==========================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <simpl.h>
#include <simplmiscProto.h>

char mem[1024];

int main(int argc, char **argv)
{
char *sender;
int n;
int *token;

if (name_attach(argv[1], NULL) == -1)
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

	token=(int *)mem;

	printf("nbytes=%d token=%d\n",n, *token);

	if(argv[2][0] == 'n') // normal
		Reply(sender, NULL, 0);
	else
		{
		sleep(120);
		Reply(sender, NULL, 0);
		}
	}

return(0);
}
