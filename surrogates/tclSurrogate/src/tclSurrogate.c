/*************************************************************
	FILE:		tclSurrogate.c

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
    $Log: tclSurrogate.c,v $
    Revision 1.3  2011/03/23 17:59:19  bobfcsoft
    removed termios.h include

    Revision 1.2  2010/01/30 11:10:47  bobfcsoft
    NO_ZOMBIE default

    Revision 1.1  2009/01/19 20:15:09  bobfcsoft
    relocated files

    Revision 1.4  2008/03/05 15:40:33  bobfcsoft
    removed log before child init

    Revision 1.3  2007/07/24 21:47:20  bobfcsoft
    new contact info

    Revision 1.2  2006/04/24 22:07:54  bobfcsoft
    added TRACK mask

    Revision 1.1.1.1  2005/03/27 11:50:46  paul_c
    Initial import

    Revision 1.10  2002/11/22 16:33:00  root
    2.0rc3

    Revision 1.9  2002/06/13 19:41:56  root
    some Linux distn's want sys/time.h for timeval

    Revision 1.8  2002/06/12 18:30:26  root
    added ack timer
    fcipc merge completed

    Revision 1.7  2001/12/15 12:29:43  root
    unblock and senders on exit

    Revision 1.6  2001/12/14 22:47:22  root
    exit on failure to write on socket

    Revision 1.5  2000/10/13 13:44:37  root
    LGPL
    and some extra reply protection

    Revision 1.4  2000/02/15 18:51:33  root
    fixed int main warning

    Revision 1.3  2000/01/21 14:17:43  root
    child cleanup on runaway

    Revision 1.2  1999/12/15 02:59:45  root
    sys/time.h replaced with time.h

    Revision 1.1  1999/11/04 15:40:05  root
    Initial revision


=======================================================

*************************************************************/
#include <stdio.h>
#include <stdlib.h> 			/* for exit etc */
#include <string.h>  			/* for memcpy etc */
#include <unistd.h>			/* for getnid */
#include <time.h>			// prototype for select
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>
//#include <termios.h>

#include "simpl.h"
#include "surroMsgs.h"

#define _ALLOC
#include "tclSurrogate.h"
#undef _ALLOC

// Prototypes
#include "tclSurroProto.h"
#include "simplProto.h"
#include "loggerProto.h"
#include "socketProto.h"

/*=========================================================
	tclSurrogate - entry point
=========================================================*/
int main(int argc, char **argv)
{
int x_it=0;

// defaults for parent process
sprintf(me,"tclSurrogate");
isThisChild=0;

// parse command line args
initialize(argc,argv);

// both process (parent and child) loop here
while(!x_it)
	{
	fcLogx(__FILE__, me,
		globalMask,
		TCL_SURROGATE_MISC,
		"top of loop child=%d my_fds[0]=%d my_fds[1]=%d",
		isThisChild,
		my_fds[0],
		my_fds[1]
		);


	if(isThisChild == 0)   // it's the parent
		{
		doParent();
		}
	else
		{
		x_it=doChild();
		}
	} // end for

unblockSenders();   // free up any reply blocked senders

fcLogx(__FILE__, me,
	globalMask,
	TCL_SURROGATE_MARK,
	"done"
	);

name_detach();
detachSocket(mySocket);

return(1);

} // end tclSurrogate

/*====================================================
	doParent - entry point
====================================================*/
int doParent()
{
static char *fn="doParent";
pid_t myChild;
static int counter=0;
int i;

inset = watchset;
select(maxfd+1, &inset, NULL, NULL, NULL);

// Is this from socket
if(FD_ISSET(my_fds[0], &inset))  //  socket is ready
	{
	fcLogx(__FILE__, fn,
		globalMask,
		TCL_SURROGATE_MISC,
		"got message on socket=%d",
		my_fds[0] 
		);

	socket_fd=acceptSocket(mySocket);

	fcLogx(__FILE__, fn,
		globalMask,
		TCL_SURROGATE_TRACK,
		"fork child socket_fd=%d",
		socket_fd
		);

	myChild = fork();
	if(myChild == 0)   // in child
		{
		isThisChild=1;
		sprintf(me,"tclSurro_%03d",counter);

		close(mySocket);

		initChild(counter);

// reset the select stuff
		my_fds[0] = socket_fd;   // socket
		my_fds[1] = whatsMyRecvfd();  // fifo
		my_fds[2] = whatsMyReplyfd(); // reply fifo
		FD_ZERO(&watchset);
		FD_SET(my_fds[0], &watchset);
		FD_SET(my_fds[1], &watchset);
		FD_SET(my_fds[2], &watchset);
		
		for(maxfd=my_fds[0],i=1; i<3; i++)
			{
			if(my_fds[i] > maxfd) maxfd=my_fds[i];
			}
		}
	else	  // in parent
		{
		close(socket_fd);

#ifdef ZOMBIE
		killZombies();
#endif

		fcLogx(__FILE__, fn,
			globalMask,
			TCL_SURROGATE_MARK,
			"in parent"
			);
		counter++;
		if(counter > 999) counter=0;
		}
			
	} // end if socket

else
// Is this from fifo
if(FD_ISSET(my_fds[1], &inset))  //  fifo is ready
	{
	fcLogx(__FILE__, fn,
		globalMask,
		TCL_SURROGATE_MISC,
		"got message on fifo=%d",
		my_fds[1] 
		);

	hndlParentFifo();
	}
return(1);
} // end doParent

/*====================================================
	doChild - entry point
====================================================*/
int doChild()
{
static char fn[30];
int x_it=0;
struct timeval tv;
int rc;

if(fn[0] != 'd')
	sprintf(fn,"doChild_%03d",myIndex);

inset = watchset;
if(isWaitingForAck() == 1)
	{
	tv.tv_sec = maxAckTime;
	tv.tv_usec = 0;
	rc=select(maxfd+1, &inset, NULL, NULL, &tv);
	}
else
	rc=select(maxfd+1, &inset, NULL, NULL, NULL);

if(rc == 0) // timeout occurred
	{
	fcLogx(__FILE__, fn,
		TCL_SURROGATE_MARK,
		TCL_SURROGATE_MARK,
		"timeout while waiting for socket ACK"
		);
	x_it=1;
	}
else
// Is this from socket
if(FD_ISSET(my_fds[0], &inset))  //  socket is ready
	{
	fcLogx(__FILE__, fn,
		globalMask,
		TCL_SURROGATE_MISC,
		"got message on socket=%d",
		my_fds[0] 
		);

	clrWaitingForAck();

	x_it=hndlSocket();
	} // end if socket

else
// Is this from fifo
if(FD_ISSET(my_fds[1], &inset))  //  fifo is ready
	{
	fcLogx(__FILE__, fn,
		globalMask,
		TCL_SURROGATE_MISC,
		"got message on fifo fd=%d",
		my_fds[1] 
		);

	if(hndlChildFifo() == -1)
		x_it=1;
	}
else

// Is this from reply fifo
if(FD_ISSET(my_fds[2], &inset))  // should never come here 
	{
	char myBuf[10];
#if 0
	FCIPC_REC *slotPtr;
	FCMSG_REC *replyPtr;
#endif

#if 0
	waitForBytes(my_fds[2], myBuf, 4);
#endif
	read(my_fds[2], myBuf,4);

#if 0
	slotPtr=pointAtSlot(whatsMySlot());
	slotPtr=_simpl_pointAtSlot(whatsMySlot());
	replyPtr=(FCMSG_REC *)slotPtr->msgBuf;

	printf("lost reply:%X-%X-%X-%X rbytes=%d\n",
		myBuf[0],
		myBuf[1],
		myBuf[2],
		myBuf[3],
		replyPtr->nbytes);
#endif
	fcLogx(__FILE__, fn,
		TCL_SURROGATE_MARK,
		TCL_SURROGATE_MARK,
		"lost reply %X-%X-%X-%X",
		myBuf[0],
		myBuf[1],
		myBuf[2],
		myBuf[3]
		);

	}


return(x_it);
} // end doChild
