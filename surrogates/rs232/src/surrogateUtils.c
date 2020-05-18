/***********************************************************************

FILE:			surrogateUtils.c

DATE:			07 July 23

DESCRIPTION:	This file contains useful utility functions used by the
				various surrogate*.c files.

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
$Log: surrogateUtils.c,v $
Revision 1.2  2010/01/31 23:27:34  bobfcsoft
ifdef killZombie function

Revision 1.1  2008/04/21 17:24:37  johnfcsoft
initial


***********************************************************************/

// application header
#define _ALLOC extern
#include "surrogate.h"
#undef _ALLOC

/**********************************************************************
FUNCTION:	int checkMemory(int, int)

PURPOSE:	Adjust the global dynamic memory for upward growth. 

RETURNS:	int	
**********************************************************************/	

int checkMemory(int msgType, int size)
{
// inMsgArea is global
// inMsgSize is global
// ouMsgArea is global
// outMsgSize is global
const static char *fn = "checkMemory_rs232";

if (msgType == IN)
	{
	if (size > inMsgSize)
		{
		// realloc memory
		inMsgArea = realloc(inMsgArea, size);
		if (!inMsgArea)
			{ 
			_simpl_log("%s: in message memory allocation error-%s\n", fn, strerror(errno));
			return(-1);
			}

		// reset global memory size
		inMsgSize = size;
		}
	}
else
	{
	if (size > outMsgSize)
		{
		// realloc memory
		outMsgArea = realloc(outMsgArea, size);
		if (!outMsgArea)
			{	 
			_simpl_log("%s: out message memory allocation error-%s\n", fn, strerror(errno));
			return(-1);
			}

		// reset global memory size
		outMsgSize = size;
		}
	}

return(0);
}

#ifdef ZOMBIE
/**********************************************************************
FUNCTION:	killZombies(void)

PURPOSE:	Forking parents make use of this call to release zombie
			children by retrieving their exit status.

			Also helps to keep the surrogate pid array current.

RETURNS:	void
**********************************************************************/	

void killZombies()
{
register int i;
pid_t pid;

for (i = 0; i < MAX_SURROGATES; i++)
	{
	pid = waitpid(-1, NULL, WNOHANG | WUNTRACED);
	if (pid <= 0)
		{
		break;
		}
	}
}
#endif
