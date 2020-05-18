/*************************************************************
	FILE:		tclSurroFifo.c

	DESCRIPTION:	
	This file contains the socket server code Tcl/Tk
	surrogate process.

	AUTHOR:			R.D. Findlay

-----------------------------------------------------------------------
    Copyright (C) 1999,2002,2007 FCSoftware Inc. 

    This software is in the public domain.
    Permission to use, copy, modify, and distribute this software and its
    documentation for any purpose and without fee is hereby granted, 
    without any conditions or restrictions.
    This software is provided "as is" without express or implied warranty.

    If you discover a bug or add an enhancement contact us on the
    SIMPL project mailing list. 

-----------------------------------------------------------------------
	Revision history:
=======================================================
    $Log: tclSurroFifo.c,v $
    Revision 1.2  2011/03/23 17:56:21  bobfcsoft
    removed termios.h include

    Revision 1.1  2009/01/19 20:15:09  bobfcsoft
    relocated files

    Revision 1.2  2007/07/24 21:47:20  bobfcsoft
    new contact info

    Revision 1.1.1.1  2005/03/27 11:50:48  paul_c
    Initial import

    Revision 1.9  2002/11/22 16:32:37  root
    2.0rc3

    Revision 1.8  2002/11/19 15:06:41  root
    2.0rc1

    Revision 1.7  2002/06/12 18:28:34  root
    fcipc merge completed
    unblock senders with ERROR

    Revision 1.6  2001/12/15 12:26:36  root
    added rememberSender, forgetSender and unblockSenders functions

    Revision 1.5  2001/12/14 22:49:31  root
    unblock sender in case of failure to relay on socket

    Revision 1.4  2000/10/13 13:42:20  root
    added LGPL

    Revision 1.3  2000/01/21 14:16:56  root
    bug fix on relay ... needed to add 4 bytes for sender ID

    Revision 1.2  1999/12/15 02:29:41  root
    replace sys/time.h with time.h

    Revision 1.1  1999/11/04 15:40:27  root
    Initial revision


=======================================================

*************************************************************/
#include <stdio.h>
#include <stdlib.h> 			/* for exit etc */
#include <string.h>  			/* for memcpy etc */
#include <unistd.h>			/* for getnid */
#include <time.h>			// prototype for select
#include <sys/types.h>
#include <fcntl.h>
//#include <termios.h>

#include "simpl.h"
#include "surroMsgs.h"

#define _ALLOC extern
#include "tclSurrogate.h"
#undef _ALLOC

// Prototypes
#include "tclSurroProto.h"
#include "simplProto.h"
#include "loggerProto.h"
#include "socketProto.h"

/*=========================================================
	hndlChildFifo - entry point
=========================================================*/
int hndlChildFifo()
{
static char fn[30];
int nbytes;
char * sender;
int myslot;
SG_RELAY_MSG *outMsg;
int rc=1;

if(fn[0] != 'c')
	sprintf(fn,"childFifo_%03d",myIndex);

fcLogx(__FILE__, fn,
	globalMask,
	TCL_SURROGATE_MISC,
	"ding"
	);

#if 0
sender = receiveMsg(&msg, &nbytes);
memcpy(inArea,msg,nbytes);
#endif
nbytes=Receive(&sender, inArea, 2048);
myslot=rememberSender(sender);

fcLogx(__FILE__, fn,
	globalMask,
	TCL_SURROGATE_MISC,
	"%d bytes from fifo sender=%d",
	nbytes,
	myslot
	);

outMsg=(SG_RELAY_MSG *)outArea;

outMsg->token=SG_RELAY_IT;
outMsg->fromWhom=myslot;
memcpy(&outMsg->dataMark,inArea,nbytes);
outMsg->nbytes=nbytes+4;  // 4 more for sender

fcLogx(__FILE__, fn,
	globalMask,
	TCL_SURROGATE_MISC,
	"RELAY nbytes=%d from %d",
	outMsg->nbytes,
	myslot
	);

rc = relayToSocket(socket_fd,
	outArea,
	outMsg->nbytes+4);  // 4 more for token and nbytes
if(rc == -1)
	{
#if 0
	loadReply(sender,NULL,0);
	replyMsg(sender);
#endif
	Reply(sender, NULL, 0);
	forgetSender(myslot);
	}
else
	setWaitingForAck();

return(rc);

} // end hndlChildFifo

/*=========================================================
	hndlParentFifo - entry point
=========================================================*/
int hndlParentFifo()
{
static char *fn="hndlParentFifo";
int nbytes;
char * fromWhom;
int rc=1;

#if 0
fromWhom = receiveMsg(&msg, &nbytes);
memcpy(inArea,msg,nbytes);
#endif
nbytes=Receive(&fromWhom,inArea,MAX_MSG_BUFFER_SIZE);

fcLogx(__FILE__, fn,
	globalMask,
	TCL_SURROGATE_MISC,
	"%d bytes from %X",
	nbytes,
	fromWhom
	);

#if 0
loadReply(fromWhom,NULL,0);
replyMsg(fromWhom);
#endif
Reply(fromWhom, NULL, 0);

return(rc);

} // end hndlParentFifo

/*=========================================================
	rememberSender - entry point
=========================================================*/
int rememberSender(char *sender)
{
static char *fn="rememberSender";
int i;
int rc=-1;

fcLogx(__FILE__, fn,
	globalMask,
	TCL_SURROGATE_FUNC_IO,
	"sender=%X",
	sender
	);

for(i=0; i<32; i++)
	{
	if(myblockedSenders[i] == NULL)
		{
		myblockedSenders[i]=sender;
		rc=i;
		break;
		}
	}

fcLogx(__FILE__, fn,
	globalMask,
	TCL_SURROGATE_FUNC_IO,
	"rc=%d",
	rc
	);

return(rc);

} // end rememberSender

/*=========================================================
	forgetSender - entry point
=========================================================*/
int forgetSender(int myslot)
{
static char *fn="forgetSender";
int rc=-1;

fcLogx(__FILE__, fn,
	globalMask,
	TCL_SURROGATE_FUNC_IO,
	"sender=%d",
	myslot
	);

myblockedSenders[myslot]=NULL;
rc=myslot;

fcLogx(__FILE__, fn,
	globalMask,
	TCL_SURROGATE_FUNC_IO,
	"rc=%d",
	rc
	);

return(rc);

} // end forgetSender

/*=========================================================
	unblockSenders - entry point
=========================================================*/
int unblockSenders()
{
static char *fn="unblockSenders";
int i;
int rc=-1;

fcLogx(__FILE__, fn,
	globalMask,
	TCL_SURROGATE_FUNC_IO,
	"ding"
	);

for(i=0; i<32; i++)
	{
	if(myblockedSenders[i] != NULL)
		{
		fcLogx(__FILE__, fn,
			globalMask,
			TCL_SURROGATE_MISC,
			"unblocking sender[%d]=%X",
			i,
			myblockedSenders[i]
			);

#if 0
		replyWithError(myblockedSenders[i]);
#endif
		ReplyError(myblockedSenders[i]);

		myblockedSenders[i]=NULL;
		rc=1;
		}
	}

fcLogx(__FILE__, fn,
	globalMask,
	TCL_SURROGATE_FUNC_IO,
	"rc=%d",
	rc
	);

return(rc);

} // end unblockSenders
