/*======================================================================

FILE:			binstr.c

DESCRIPTION:	This program contains the source for the binhex library.

AUTHOR:			FC Software Inc.
-----------------------------------------------------------------------
    Copyright (C) 2000, 2002 FCSoftware Inc. 

    This software is in the public domain.
    Permission to use, copy, modify, and distribute this software and its
    documentation for any purpose and without fee is hereby granted, 
    without any conditions or restrictions.
    This software is provided "as is" without express or implied warranty.

    If you discover a bug or add an enhancement contact us on the 
    SIMPL project mailing list.

-----------------------------------------------------------------------

Revision history:
====================================================================
$Log: binstr.c,v $
Revision 1.2  2007/07/24 19:52:40  bobfcsoft
new contact info

Revision 1.1.1.1  2005/03/27 11:50:58  paul_c
Initial import

Revision 1.1  2003/04/14 14:13:31  root
Initial revision


====================================================================
======================================================================*/

#include <string.h>
#include "simplmiscProto.h"
#include "binstr.h"

/********************************************************************/
/*			Binary to Hex Character	Library Routines				*/
/********************************************************************/

void btosUI(unsigned int number, char *str, int clip)
{
CUI u;

u.n = number;

btos4(&u.m, str, clip);
}

/*-------------------------------------------------------------------*/

void btosSI(signed int number, char *str, int clip)
{
CSI u;

u.n = number;

btos4(&u.m, str, clip);
}

/*-------------------------------------------------------------------*/

void btosUSI(unsigned short int number, char *str, int clip)
{
CUSI u;

u.n = number;

btos2(&u.m, str, clip);
}

/*-------------------------------------------------------------------*/

void btosSSI(signed short int number, char *str, int clip)
{
CSSI u;

u.n = number;

btos2(&u.m, str, clip);
}

/*-------------------------------------------------------------------*/

void btosF(float number, char *str, int clip)
{
CF u;

u.n = number;

btos4(&u.m, str, clip);
}

/*-------------------------------------------------------------------*/

void btos4(unsigned int *ptr, char *str, int clip)
{
register int i;
int limit = 7;

if ( clip && (clip < 4) )
	{
	limit = (clip << 1) - 1;
	}

for (i = limit; i >= 0; i--, *ptr >>= 4)
	{
	str[i] = *ptr % 16;
	}
}

/*-------------------------------------------------------------------*/

void btos2(unsigned short int *ptr, char *str, int clip)
{
register int i;
int limit = 3;

if ( clip && (clip < 2) )
	{
	limit = (clip << 1) - 1;
	}

for (i = limit; i >= 0; i--, *ptr >>= 4)
	{
	str[i] = *ptr % 16;
	}
}

/********************************************************************/
/*			Hex Character to Binary	Library Routines				*/
/********************************************************************/

unsigned int stobUI(char *str, int clip)
{
CUI u;

u.m = 0;

stob4(&u.m, str, clip);

return(u.n);
}

/*-------------------------------------------------------------------*/

signed int stobSI(char *str, int clip)
{
CSI u;

if ( clip && (str[0] == 'F') )
	{
	memset((unsigned int *)&u.m, 0xFF, sizeof(signed short int));
	}
else
	{
	u.m = 0;
	}

stob4(&u.m, str, clip);

return(u.n);
}

/*-------------------------------------------------------------------*/

unsigned short int stobUSI(char *str, int clip)
{
CUSI u;

u.m = 0;

stob2(&u.m, str, clip);

return(u.n);
}

/*-------------------------------------------------------------------*/

signed short int stobSSI(char *str, int clip)
{
CSSI u;

if ( clip && (str[0] == 'F') )
	{
	memset((unsigned short int *)&u.m, 0xFF, sizeof(signed short int));
	}
else
	{
	u.m = 0;
	}

stob2(&u.m, str, clip);

return(u.n);
}

/*-------------------------------------------------------------------*/

float stobF(char *str, int clip)
{
CF u;

if ( (str[0] == 'F') )
	{
	memset((unsigned int *)&u.m, 0xFF, sizeof(float));
	}
else
	{
	u.m = 0;
	}

stob4(&u.m, str, clip);

return(u.n);
}

/*-------------------------------------------------------------------*/

void stob4(unsigned int *ptr, char *str, int clip)
{
register int i;
int limit = 8;

if ( clip && (clip < 4) )
	{
	limit = (clip << 1);
	}

for (i = 0; i < limit; i++)
	{
	*ptr <<= 4;
	*ptr += str[i];
	}
}

/*-------------------------------------------------------------------*/

void stob2(unsigned short int *ptr, char *str, int clip)
{
register int i;
int limit = 4;

if ( clip && (clip < 2) )
	{
	limit = (clip << 1);
	}

for (i = 0; i < limit; i++)
	{
	*ptr <<= 4;
	*ptr += str[i];
	}
}
