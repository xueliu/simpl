/***********************************************************************

FILE:			surrogateInit.c

DATE:			05 Oct 28

DESCRIPTION:	This file contains the initialize procedures used by
				surrogate.c.

AUTHOR:			FC Software Inc.
-----------------------------------------------------------------------
    Copyright (C) 2005 FCSoftware Inc. 

    This software is in the public domain.
    Permission to use, copy, modify, and distribute this software and its
    documentation for any purpose and without fee is hereby granted, 
    without any conditions or restrictions.
    This software is provided "as is" without express or implied warranty.

    If you discover a bug or add an enhancement here's how to reach us: 

	fcsoft@allstream.net
-----------------------------------------------------------------------

REVISIONS:
$Log: surrogateInit.c,v $
Revision 1.3  2011/09/08 17:02:21  bobfcsoft
more portable arg processing

Revision 1.2  2007/01/09 15:23:59  bobfcsoft
restore from tcp_x

Revision 1.3  2006/04/24 22:15:23  bobfcsoft
added b_port

Revision 1.2  2005/12/20 15:26:18  bobfcsoft
more 3.0 prerelease code

Revision 1.1  2005/11/11 20:51:14  bobfcsoft
prerelease for 3.0

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
char *me = "intialize_tcp";
register int i;

// memArea is global variable used for messaging
memArea = NULL;

// a_port is a global variable
// A_PORT is the default macro set in surrogate.h
a_port = A_PORT;
b_port = a_port;

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
			case 'a':
	  			if (*++p == 0)
					{
					p++;
					}
//				a_port = atoi(p); 
				a_port = atoi(argv[++i]); 
				break;

			case 'b':
				if (*++p == 0)
					{
					p++;
					}
//				b_port = atoi(p); 
				b_port = atoi(argv[++i]); 
				break;

			case 'i':
				if (*++p == 0)
					{
					p++;
					}
//				intWidth = atoi(p); 
				intWidth = atoi(argv[++i]); 
				break;

			case 'k':
				if (*++p == 0)
					{
					p++;
					}
//				kaTimeout = atoi(p); 
				kaTimeout = atoi(argv[++i]); 
				break;

			case 'n':
				if (*++p == 0)
					{
					p++;
					}
//				namelocTimeout = atoi(p); 
				namelocTimeout = atoi(argv[++i]); 
				break;

			case 'r':
				if (*++p == 0)
					{
					p++;
					}
				// no longer used
				break;

			case 't':
				if (*++p == 0)
					{
					p++;
					}
				// no longer used
				break;

			default:
				_simpl_log("%s: bad command line arg `%c'\n", me, *p);
				return(-1);
			}
		}
	}	

return(0);
}
