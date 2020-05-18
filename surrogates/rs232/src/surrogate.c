/***********************************************************************

FILE:			surrogate.c

DATE:			07 July 19

DESCRIPTION:	This program immediately forks into two separate
				programs: viz. surrogate_R and surrogate_S.

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
$Log: surrogate.c,v $
Revision 1.1  2008/04/21 17:19:52  johnfcsoft
initial


***********************************************************************/

// application header
#define _SURROGATE_RS232_PRIMARY
#define _ALLOC
#include "surrogate.h"
#undef _ALLOC
#undef _SURROGATE_RS232_PRIMARY

int main(int argc, char **argv)
{
pid_t childPid;
const char *fn = "surrogate_rs232";

// allow for command line overrides
initialize(argc, argv);

// fork off into surrogate receiver and sender parent processes 
childPid = fork();
if (childPid < 0) // failure
	{
	_simpl_log("%s: cannot fork-%s\n", fn, strerror(errno));
	}
else if (childPid == 0) // child
	{
	surrogate_R();
	}
else // parent
	{
	surrogate_S();
	}

return(0);
}
