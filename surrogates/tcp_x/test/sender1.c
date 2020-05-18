/*===================================

 suitable for testing for orphaned
 surrogates

 sender simplname receivername options

 where options are
  	nl - name_locate only
  	s - send a single message

==================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <simpl.h>

#define LIMIT 1

int main(int arc, char **argv)
{
int receiver;
char mem[1024];
int *token;
int ret;
int i;

if (name_attach(argv[1], NULL) == -1)
	{
	printf("%s: cannot attach name: %s\n", argv[1], whatsMyError());
	exit(-1);
	}

receiver = name_locate(argv[2]);
if (receiver == -1)
	{
	printf("%s: cannot locate receiver: %s\n", argv[2], whatsMyError());
	exit(-1);
	}

if(argv[3][0] != 'n')
	{
	token=(int *)mem;
	*token=42;

printf("sleeping for 30s\n");
sleep(30);
	
	for (i = 1; i <= LIMIT; i++)
		{
printf("sending message\n");
		ret = Send(receiver, mem, mem, 1024, 1024);
		if (ret == -1)
			{
			printf("%s: Send error: %s\n", argv[1], whatsMyError());
			//exit(-1);
			}
		}
	}

sleep(300);

return(0);
}
