/***********************************************************************

FILE:			protocolRouter.c

DATE:			05 Nov 25

DESCRIPTION:	This program accepts "surrogate ready" messages
				and routes a name_locate() call to the desired
				protocal surrogate.
-----------------------------------------------------------------------
    Copyright (C) 2005 FCSoftware Inc. 

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    If you discover a bug or add an enhancement here's how to reach us: 

	fcsoft@allstream.net
-----------------------------------------------------------------------

REVISIONS:
$Log: protocolRouter.c,v $
Revision 1.6  2007/02/08 13:04:26  bobfcsoft
removed temporary logging

Revision 1.5  2006/04/24 22:13:30  bobfcsoft
remove temporary logs

Revision 1.4  2006/01/10 15:31:24  bobfcsoft
v3.0 changes

***********************************************************************/

// standard headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// application headers
#include <simpl.h>
#include <simplDefs.h>
#include <simplLibProto.h>
#include <simplProtocols.h>
#include <simplNames.h>
#include <surMsgs.h>

// macros
#define MAX_BUFFER_SIZE		1024

// function prototypes
int modifyProtocolTable(char *);
int hndlProtocolRequest(char *);
int hndlProtocolDump(char *);

// globals
char inArea[MAX_BUFFER_SIZE];
char outArea[MAX_BUFFER_SIZE];
PROTOCOL_TABLE_ENTRY protocolTable[MAX_NUM_PROTOCOLS];

int main()
{
const char *fn = PROTOCOL_ROUTER;
SUR_MSG_HDR *hdr;
unsigned msgSize;
register int i;
char *sender;

// attach a name for local IPC
if (name_attach(fn, NULL) == -1)
	{
	_simpl_log("%s: cannot attach name-%s\n", fn, whatsMyError());
	exit(-1);
	}

// initialize the protocol table
for (i = 0; i < MAX_NUM_PROTOCOLS; i++)
	{
	memset(protocolTable[i].protocolName, 0, MAX_PROTOCOL_NAME_LEN + 1);
	memset(protocolTable[i].programName, 0, MAX_PROGRAM_NAME_LEN + 1);
	protocolTable[i].sender = (char *)NULL;
	}

while (1)
	{
	// message origin is a name_locate() call
	msgSize = Receive(&sender, inArea, MAX_BUFFER_SIZE);
	if (msgSize == -1)
		{
		_simpl_log("%s: Receive error-%s\n", fn, whatsMyError());
		exit(-1);
		}

	hdr = (SUR_MSG_HDR *)inArea;

	// decide what to do based on the message token
	switch (hdr->token)
		{
		case SUR_SURROGATE_READY:
			modifyProtocolTable(sender);
			break;

		case SUR_REQUEST_PROTOCOL:
			hndlProtocolRequest(sender);
			break;

		case SUR_DUMP_TABLE:
			hndlProtocolDump(sender);
			break;

		default:
			_simpl_log("%s :unknown message token=%d; message size=%d\n",
							fn,
							hdr->token,
							msgSize);
		}
	}

return(0);
}
		
/**********************************************************************
FUNCTION:	modifyProtocolTable(char *)

PURPOSE:	add/remove a surrogate to/from the protocol table 

RETURNS:	int: 0=success, -1=failure
**********************************************************************/	

int modifyProtocolTable(char *sender)
{
//const static char *fn = "modifyProtocolTable";
register int i;
SUR_PROTOCOL_MSG *in;

// line up on the incoming message
in = (SUR_PROTOCOL_MSG *)inArea;

// add this surrogate protocol to the table
for (i = 0; i < MAX_NUM_PROTOCOLS; i++)
	{
	if (protocolTable[i].protocolName[0] == 0)
		{
		strcpy(protocolTable[i].protocolName, in->protocolName);
		strcpy(protocolTable[i].programName, in->programName);
		protocolTable[i].sender = sender;
		break;
		}
	}

return(0);
}

/**********************************************************************
FUNCTION:	hndlProtocolRequest(char *)

PURPOSE:	returns the name of the surrogate child which controls
			simpl communications via the requested protocol. 

RETURNS:	int: 0=success, -1=failure
**********************************************************************/	

int hndlProtocolRequest(char *sender)
{
const static char *fn = "hndlprotocolRequest";
register int i;
SUR_REQUEST_PROTOCOL_MSG *in;
SUR_PROTOCOL_REPLY_MSG *out;

// line up on incomingmemory
in = (SUR_REQUEST_PROTOCOL_MSG *)inArea;

// line up on outgoing memory
out = (SUR_PROTOCOL_REPLY_MSG *)outArea;

// initialize reply in case of failure
memset(out->programName, 0, MAX_PROGRAM_NAME_LEN + 1);

if (!strcmp(in->protocolName, SIMPL_DEFAULT))
	{
	// take the first available protocol surrogate in the table	
	for (i = 0; i < MAX_NUM_PROTOCOLS; i++)
		{
		if (strlen(protocolTable[i].protocolName))
			{
			// set reply message		
			strcpy(out->programName, protocolTable[i].programName);
			// reply to blocked protocol parent sender (such as surrogate_R for TCP) 
			Reply(protocolTable[i].sender, NULL, 0);
			// clear the entry from the table
			memset(protocolTable[i].protocolName, 0, MAX_PROTOCOL_NAME_LEN + 1);
			memset(protocolTable[i].programName, 0, MAX_PROGRAM_NAME_LEN + 1);
			protocolTable[i].sender = (char *)NULL;
			break;
			}
		}
	}
else
	{
	// find a match in the table if possible
	for (i = 0; i < MAX_NUM_PROTOCOLS; i++)
		{
		if (!strcmp(protocolTable[i].protocolName, in->protocolName))
			{
			// set reply message		
			strcpy(out->programName, protocolTable[i].programName);
			// reply to blocked protocol parent sender (such as surrogate_R for TCP) 
			Reply(protocolTable[i].sender, NULL, 0);
			// clear the entry from the table
			memset(protocolTable[i].protocolName, 0, MAX_PROTOCOL_NAME_LEN + 1);
			memset(protocolTable[i].programName, 0, MAX_PROGRAM_NAME_LEN + 1);
			protocolTable[i].sender = (char *)NULL;
			break;
			}
		}
	}

// reply to request
if (Reply(sender, out, sizeof(SUR_PROTOCOL_REPLY_MSG)) == -1)
	{
	_simpl_log("%s: bad protocol request reply to sender-%s\n", fn, whatsMyError());
	return(-1);
	}

return(0);
}

/**********************************************************************
FUNCTION:	hndlProtocolDump(char *)

PURPOSE:	returns the contects of the protocol table

RETURNS:	int: 0=success, -1=failure
**********************************************************************/	

int hndlProtocolDump(char *sender)
{
const static char *fn = "hndlProtocolDump";
SUR_DUMP_TABLE_REPLY_MSG *out;

out = (SUR_DUMP_TABLE_REPLY_MSG *)outArea;

// copy the protocol table into the reply message
memcpy(&(out->table), &protocolTable, sizeof(protocolTable));

// reply the table contents
if (Reply(sender, out, sizeof(SUR_DUMP_TABLE_REPLY_MSG)) == -1)
	{
	_simpl_log("%s: bad table dump reply to sender-%s\n", fn, whatsMyError());
	return(-1);
	}

return(0);
}
