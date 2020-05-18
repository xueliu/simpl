/*************************************************************

FILE:		sandboxRelay.c

DESCRIPTION:	
This program is an example of a relay softwareIC. 

AUTHOR:			R.D. Findlay

-----------------------------------------------------------------------
    Copyright (C) 2007 SIMPL Project. 

    This software is in the public domain.
    Permission to use, copy, modify, and distribute this software and its
    documentation for any purpose and without fee is hereby granted, 
    without any conditions or restrictions.
    This software is provided "as is" without express or implied warranty.

    If you discover a bug or add an enhancement contact us on the SIMPL
    project mailing list. 

-----------------------------------------------------------------------
Revision history:
=======================================================
$Log: sandboxRelay.c,v $
Revision 1.1  2007/07/03 18:28:04  bobfcsoft
added sandboxRelay

=======================================================

*************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define _ALLOC
#include "sandboxRelay.h"
#undef _ALLOC

#include "sandboxRelayProto.h"
#include "simplProto.h"
#include "loggerProto.h"

/*===============================================
	sandboxRelay - entry point
===============================================*/
int main(int argc, char **argv, char **envp)
{
static char *fn="sbRelay";
int x_it=0;
int nbytes;			
int rbytes=0;			
char *sender;

initialize(argc, argv);

fcLogx(__FILE__, fn,
	globalMask,
	TRACE_MARK,
	"starting"
	);

while(!x_it)
	{
	nbytes = Receive(&sender, outArea, MAX_MSG_SIZE);

	rbytes = sbSend(recvID,outArea,inArea,nbytes, MAX_MSG_SIZE);

	Reply(sender,inArea,rbytes);
	} // end while

fcLogx(__FILE__, fn,
	globalMask,
	TRACE_FUNC_IO,
	"done");

name_detach();

exit(0);

}// end sandboxRelay
