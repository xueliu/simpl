// This program works with sender0.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <simpl.h>
#include <simplmiscProto.h>

void showMessage(void);

typedef struct
	{
	int token;
	char john0[50];
	unsigned short int john1;
	signed short int john2;
	unsigned int john3;
	signed int john4;
	float john5;
	} TEST_STRUCT_0;

/*
this structure is the hex char reflection of the above structure
note that each member is twice the byte size with the exception of
the string called john0
*/
typedef struct
	{
	char token[8];
	char john0[50];
	char john1[4];
	char john2[4];
	char john3[8];
	char john4[8];
	char john5[8];
	} TEST_STRUCT_1;

char mem[1024];
TEST_STRUCT_0 var0;

int main(int argc, char **argv)
{
char *sender;
int n;
TEST_STRUCT_1 *var1;

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

	var1 = (TEST_STRUCT_1 *)mem;

	// get the token, all clips are 0 
	var0.token = stobSI(var1->token, 0);

	if (var0.token == 42)
		{
		strcpy(var0.john0, var1->john0); 
		var0.john1 = stobUSI(var1->john1, 0);
		var0.john2 = stobSSI(var1->john2, 0);
		var0.john3 = stobUI(var1->john3, 0);
		var0.john4 = stobSI(var1->john4, 0);
		var0.john5 = stobF(var1->john5, 0);

		showMessage();
		}

	Reply(sender, NULL, 0);
	}

return(0);
}

void showMessage()
{
printf("john0=%s\n", var0.john0);
printf("john1=%u\n", var0.john1);
printf("john2=%d\n", var0.john2);
printf("john3=%u\n", var0.john3);
printf("john4=%d\n", var0.john4);
printf("john5=%f\n", var0.john5);
}
