/*======================================================================

FILE:			testbinstr.c

DESCRIPTION:	This program tests the binstr library.

AUTHOR:			FC Software Inc.
-----------------------------------------------------------------------
    Copyright (C) 2000, 2002 FCSoftware Inc. 

    This software is in the public domain.
    Permission to use, copy, modify, and distribute this software and its
    documentation for any purpose and without fee is hereby granted, 
    without any conditions or restrictions.
    This software is provided "as is" without express or implied warranty.

    If you discover a bug or add an enhancement here's how to reach us: 

	fcsoft@attcanada.ca
-----------------------------------------------------------------------

Revision history:
====================================================================
$Log: testbinstr.c,v $
Revision 1.1.1.1  2005/03/27 11:50:58  paul_c
Initial import

Revision 1.1  2003/04/14 13:32:08  root
Initial revision


====================================================================
======================================================================*/

#include <stdio.h>
#include "simplmiscProto.h" 

void test_UI(void);
void test_SI(void);
void test_USI(void);
void test_SSI(void);
void test_F(void);

int main()
{
test_UI(); 
test_SI(); 
test_USI(); 
test_SSI(); 
test_F(); 

return(1);
}

/********************************************************************/
/*							Test Routines							*/
/********************************************************************/

void test_UI()
{
unsigned int number = 1234567;
char str[8];

btosUI(number, str, 0);

printf("number=%d str=%d-%d-%d-%d-%d-%d-%d-%d\n",
			number,
			str[0],
			str[1],
			str[2],
			str[3],
			str[4],
			str[5],
			str[6],
			str[7]);

number = stobUI(str, 0);
printf("number=%d\n", number); 
}

/*-------------------------------------------------------------------*/

void test_SI()
{
signed int number = -214748364;
char str[8];

btosSI(number, str, 0);

printf("number=%d str=%d-%d-%d-%d-%d-%d-%d-%d\n",
			number,
			str[0],
			str[1],
			str[2],
			str[3],
			str[4],
			str[5],
			str[6],
			str[7]);

number = stobSI(str, 0);
printf("number=%d\n", number); 
}

/*-------------------------------------------------------------------*/

void test_USI()
{
unsigned short int number = 50000;
char str[4];

btosUSI(number, str, 0);

printf("number=%d str=%d-%d-%d-%d\n",
			number,
			str[0],
			str[1],
			str[2],
			str[3]);

number = stobUSI(str, 0);
printf("number=%d\n", number); 
}

/*-------------------------------------------------------------------*/

void test_SSI()
{
signed short int number = -2001;
char str[4];

btosSSI(number, str, 0);

printf("number=%d str=%d-%d-%d-%d\n",
			number,
			str[0],
			str[1],
			str[2],
			str[3]);

number = stobSSI(str, 0);
printf("number=%d\n", number); 
}

/*-------------------------------------------------------------------*/

void test_F()
{
float number = -2001.98;
char str[8];

btosF(number, str, 0);

printf("number=%f str=%d-%d-%d-%d-%d-%d-%d-%d\n",
			number,
			str[0],
			str[1],
			str[2],
			str[3],
			str[4],
			str[5],
			str[6],
			str[7]);

number = stobF(str, 0);
printf("number=%f\n", number); 
}
