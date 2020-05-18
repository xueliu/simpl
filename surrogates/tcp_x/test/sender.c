// This program works with receiver0.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <simpl.h>
#include <simplmiscProto.h>

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

void showTime(void);
void showMessage(void);

struct timeval start;
struct timeval stop;
TEST_STRUCT_0 var0;
TEST_STRUCT_1 var1;

int main(int arc, char **argv)
{
int receiver;
char mem[1024];
int ret;

// load the initial structure
var0.token = 42;
strcpy(var0.john0, "Mary had a litle lamb");
var0.john1 = 37;
var0.john2 = -37;
var0.john3 = 1037;
var0.john4 = -1037;
var0.john5 = 42.420000;

// convert to hex char protocol
// NOTE: the clip numbers are 0 because we are 32-bit sending to 32-bit
btosSI(var0.token, var1.token, 0);
strcpy(var1.john0, var0.john0); 
btosUSI(var0.john1, var1.john1, 0);
btosSSI(var0.john2, var1.john2, 0);
btosUI(var0.john3, var1.john3, 0);
btosSI(var0.john4, var1.john4, 0);
btosF(var0.john5, var1.john5, 0);

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

// get start time
gettimeofday(&start, NULL);

ret = Send(receiver, &var1, mem, sizeof(TEST_STRUCT_1), 1024);
if (ret == -1)
	{
	printf("%s: Send error: %s\n", argv[1], whatsMyError());
	exit(-1);
	}

// get stop time
gettimeofday(&stop, NULL);

showTime();
showMessage();

return(0);
}

void showTime()
{
time_t total;

total = (stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_usec - start.tv_usec);
printf("time taken=%lu us\n", total);
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
