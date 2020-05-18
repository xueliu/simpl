/*======================================================================

FILE:			dumpProtocolTable.c

DESCRIPTION:	This program sends for and displays the contents
				of the protocolRouter's protocol table.  

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
$Log: dumpProtocolTable.c,v $
Revision 1.2  2007/07/24 19:52:40  bobfcsoft
new contact info

Revision 1.1.1.1  2005/03/27 11:50:58  paul_c
Initial import

Revision 1.1  2002/11/22 16:37:22  root
Initial revision


====================================================================
======================================================================*/

// system headers
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// simpl headers 
#include <simpl.h>
#include <simplProtocols.h>
#include <simplNames.h>
#include <surMsgs.h>

int main()
{
const static char *fn = "dumpProtocolTable";
register int i;
SUR_DUMP_TABLE_MSG outMsg;
SUR_DUMP_TABLE_REPLY_MSG inMsg;
int rc; 

if (name_attach(fn, NULL) == -1)
	{
	printf("%s: cannot attach name: %s\n", fn, whatsMyError());
	exit(-1);
	}

rc = name_locate(PROTOCOL_ROUTER);
if (rc == -1)
	{
	printf("%s: cannot locate local protocol router: %s\n", fn, whatsMyError());
	exit(-1);
	}

// build message to protocol router
outMsg.hdr.token = SUR_DUMP_TABLE;
outMsg.hdr.nbytes = sizeof(SUR_DUMP_TABLE_MSG); 

// send name locate message to the protocol router
if (Send(rc, &outMsg, &inMsg, sizeof(SUR_DUMP_TABLE_MSG), sizeof(SUR_DUMP_TABLE_REPLY_MSG)) == -1)
	{
	printf("%s: cannot send to protocol router: %s\n", fn, whatsMyError());
	exit(-1);
	}

// add to the remote receiver offset table
printf("\nMaximum number of protocols=%d\n", MAX_NUM_PROTOCOLS);
printf("Default protocol=%-19s\n", inMsg.table[0].protocolName); 
printf("Default surrogate=%-19s\n\n", inMsg.table[0].programName); 
		
printf("Entry        Protocol         Protocol Surrogate\n");
printf("=====   ===================   ===================\n");
for (i = 0; i < MAX_NUM_PROTOCOLS; i++)
	{
	printf("  %2d    %-19s   %-19s\n",
		i+1, inMsg.table[i].protocolName, inMsg.table[i].programName); 
	}

return(0);
}
