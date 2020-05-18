/***********************************************************************

FILE:			surrogateInit.c

DATE:			07 July 19

DESCRIPTION:	This file contains the initialize procedures used by
				surrogate_232.c.

AUTHOR:			FC Software Inc.
-----------------------------------------------------------------------
    Copyright (C) 2005 FCSoftware Inc. 

    This software is in the public domain.
    Permission to use, copy, modify, and distribute this software and its
    documentation for any purpose and without fee is hereby granted, 
    without any conditions or restrictions.
    This software is provided "as is" without express or implied warranty.

    If you discover a bug or add an enhancement contact us on the
    SIMPL project mailing list.
-----------------------------------------------------------------------
REVISIONS:
$Log: surrogateInit.c,v $
Revision 1.1  2008/04/21 17:20:40  johnfcsoft
initial


***********************************************************************/

// application header
#define _ALLOC extern
#include "surrogate.h"
#undef _ALLOC

/**********************************************************************
FUNCTION:	initialize(int argc, char **argv)

PURPOSE:	read command line parameters

RETURNS:	int: 0=success, -1=failure
**********************************************************************/	

int initialize(int argc, char **argv)
{
char *fn = "intialize_rs232";
register int i;

// inArea is global variable used for incoming messages
inMsgArea = NULL;
inMsgSize = 0;

// outArea is global variable used for outgoing messages
outMsgArea = NULL;
outMsgSize = 0;

// intWidth is a global variable which is 4 for 32-bit and 8 for 64-bit
// INT_WIDTH_32 is the default macro set in surrogate.h
intWidth = INT_WIDTH_32;

// namelocTimeout is a global variable measured in seconds
// this is the maximum time allowed to negotiate a remote name locate call
// NAMELOC_TIMEOUT is the default macro set in surrogate.h
namelocTimeout = NAMELOC_TIMEOUT;

// kaTimeout is a global variable measured in seconds
// a value of zero turns off the keep alive functionality
// KEEP_ALIVE_TIMEOUT is the default macro set in surrogate.h
kaTimeout = KEEP_ALIVE_TIMEOUT;

// process command line arguments (if any)
for (i = 1; i <= argc; ++i) 
	{	
	char *p = argv[i];

	if (p == NULL)
		{
		continue;
		}

	if (*p == '-')
		{
		switch (*++p)
			{
			case 'i':
				if (*++p == 0)
					{
					p++;
					}
				intWidth = atoi(p); 
				break;

			case 'k':
				if (*++p == 0)
					{
					p++;
					}
				kaTimeout = atoi(p); 
				break;

			case 'n':
				if (*++p == 0)
					{
					p++;
					}
				namelocTimeout = atoi(p); 
				break;

			default:
				_simpl_log("%s: bad command line arg `%c'\n", fn, *p);
				return(-1);
			}
		}
	}	

return(0);
}
