/***********************************************************************

FILE:			surrogate.c

DATE:			05 Oct 31

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
Revision 1.3  2008/04/18 15:35:04  bobfcsoft
fixed header block

Revision 1.2  2007/01/09 15:23:23  bobfcsoft
restore from tcp_x

Revision 1.2  2005/12/20 15:26:18  bobfcsoft
more 3.0 prerelease code

Revision 1.1  2005/11/11 20:51:14  bobfcsoft
prerelease for 3.0

***********************************************************************/

// application header
#define _SURROGATE_TCP_PRIMARY
#define _ALLOC
#include "surrogate.h"
#undef _ALLOC
#undef _SURROGATE_TCP_PRIMARY

int main(int argc, char **argv)
{
pid_t childPid;
const char *fn = "surrogate_tcp";

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
