/*************************************************************
	FILE:		tclSurroSock.c

	DESCRIPTION:	
	This file contains the socket server code Tcl/Tk
	surrogate process.

	AUTHOR:			R.D. Findlay

-----------------------------------------------------------------------
    Copyright (C) 1999, 2002, 2006 FCSoftware Inc. 

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
    $Log: tclSurroSocket.c,v $
    Revision 1.2  2011/03/23 17:58:13  bobfcsoft
    removed termios.h include

    Revision 1.1  2009/01/19 20:15:09  bobfcsoft
    relocated files

    Revision 1.4  2007/07/24 21:47:20  bobfcsoft
    new contact info

    Revision 1.3  2007/03/07 17:52:39  bobfcsoft
    trusted network change

    Revision 1.2  2006/02/07 17:34:48  bobfcsoft
    Tcl/Tk enhancements

    Revision 1.1.1.1  2005/03/27 11:50:46  paul_c
    Initial import

    Revision 1.12  2003/03/07 13:49:22  root
    using workArea for reply in SEND_IT case

    Revision 1.11  2002/11/22 16:32:01  root
    2.0rc3

    Revision 1.10  2002/11/19 15:13:47  root
    2.0rc1

    Revision 1.9  2002/06/12 18:29:53  root
    handle ACK and PING
    fcipc merge completed

    Revision 1.8  2001/12/15 12:29:04  root
    remove blocked sender when reply goes out

    Revision 1.7  2000/10/13 13:43:45  root
    LGPL
    expanded to 2k message size

    Revision 1.6  2000/04/17 15:00:42  root
    SEND_NO_REPLY added

    Revision 1.5  2000/01/24 21:51:47  root
    VC_LOGIT is now non blocking call via socket
    ie. no reply required

    Revision 1.4  2000/01/21 14:18:27  root
    trap runaway and exit cleanly

    Revision 1.3  1999/12/30 12:11:48  root
    expanded nameAttach logic to return name,pid and rc
    made compatible with cleaned up message structure

    Revision 1.2  1999/12/15 02:58:51  root
    sys/time.h replaced with time.h

    Revision 1.1  1999/11/04 15:40:12  root
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

#include "surroMsgs.h"

#define _ALLOC extern
#include "tclSurrogate.h"
#undef _ALLOC

// Prototypes
#include "tclSurroProto.h"
#include "simplProto.h"
#include "loggerProto.h"
#include "socketProto.h"
int myExit();

/*=========================================================
	hndlSocket - entry point
=========================================================*/
int hndlSocket()
{
static char fn[30];
SG_SEND_MSG *inMsg;
int x_it=0;
int sender;

if(fn[0] != 'h')
	sprintf(fn,"hndlSock_%03d",myIndex);

inMsg=(SG_SEND_MSG *)inArea;

sender = receiveFromSocket(socket_fd, inArea);

fcLogx(__FILE__, fn,
	globalMask,
	TCL_SURROGATE_MISC,
	"from socket sender=%d token=%d",
	sender,
	inMsg->token
	);

if(sender == -1)
	x_it=1;
else
switch(inMsg->token)
	{
	case SG_NAME_ATTACH:
		{
		SG_NAME_ATTACH_MSG *myMsg;
		SG_NAME_ATTACH_MSG *replyMsg;

		myMsg = (SG_NAME_ATTACH_MSG *)&inMsg->dataMark;

		if(myMsg->myName[0] != 0)
			{
			if(trustedNetworkFlag == 1 && myMsg->myPid != 0)
				{
				slayProcess(myMsg->myName, myMsg->myPid);
				}

			name_detach();
			sprintf(childsName,"%.19s",myMsg->myName);

			childsSlot=name_attach(myMsg->myName, (void *)myExit);
			if(childsSlot == -1)
				{
				replyMsg=(SG_NAME_ATTACH_MSG *)workArea;
				sprintf(replyMsg->myName,"%.19s",childsName);
				replyMsg->myPid = -1;
				replyMsg->mySlot = childsSlot;
				replyToSocket(sender,workArea,sizeof(SG_NAME_ATTACH_MSG));

// no point in continuing here because my name attach failed
// even fcLogx will now fail because they wrap SIMPL operations
// such as Send()
				x_it=1;  
				return(x_it);
				}

			fcLogx(__FILE__, fn,
				globalMask,
				TCL_SURROGATE_MISC,
				"NAME_ATTACH as <%s> slot=%d",
				myMsg->myName,
				childsSlot
				);

// reset the select stuff
			my_fds[1] = whatsMyRecvfd();  // fifo
			FD_ZERO(&watchset);
			FD_SET(my_fds[0], &watchset);
			FD_SET(my_fds[1], &watchset);
			maxfd = my_fds[0] > my_fds[1] ? my_fds[0]: my_fds[1]; 
			} // if name is supplied

		replyMsg=(SG_NAME_ATTACH_MSG *)workArea;
		sprintf(replyMsg->myName,"%.19s",childsName);
		replyMsg->myPid = getpid();
		replyMsg->mySlot = childsSlot;
		replyToSocket(sender,workArea,sizeof(SG_NAME_ATTACH_MSG));

		fcLogx(__FILE__, fn,
			globalMask,
			TCL_SURROGATE_MISC,
			"NAME_ATTACH myName=<%s> myPid=%d myslot=%d",
			replyMsg->myName,
			replyMsg->myPid,
			replyMsg->mySlot
			);
		}
		break;

	case SG_NAME_DETACH:
		{
		fcLogx(__FILE__, fn,
			globalMask,
			TCL_SURROGATE_MISC,
			"NAME_DETACH"
			);

		replyToSocket(sender,NULL,0);
		sleep(2);
		close(socket_fd);

		x_it=1;
		}
		break;

	case SG_NAME_LOCATE:
		{
		SG_NAME_LOCATE_MSG *myMsg;
		SG_NAME_LOCATE_MSG *replyMsg;
		int who;
		int rc=-1;
	
		myMsg = (SG_NAME_LOCATE_MSG *)&inMsg->dataMark;

		who=name_locate(myMsg->thisName);
		if(who != -1)
			rc = who;  // fd is all that is sent back

		fcLogx(__FILE__, fn,
			globalMask,
			TCL_SURROGATE_MISC,
			"NAME_LOCATE <%s> rc=%d",
			myMsg->thisName,
			rc
			);

		replyMsg=(SG_NAME_LOCATE_MSG *)workArea;
		replyMsg->rc = rc;
		replyToSocket(sender,workArea,sizeof(SG_NAME_LOCATE_MSG));
		}
		break;

	case SG_SEND_IT:
	case SG_SEND_NO_REPLY:
		{
		SG_SEND_MSG *myMsg;
		int msglen;
		int rbytes;
		UINT16 myToken;
	
		myMsg = (SG_SEND_MSG *)inArea;
		msglen = myMsg->nbytes-sizeof(int);
		myToken = myMsg->token;

		fcLogx(__FILE__, fn,
			globalMask,
			TCL_SURROGATE_MISC,
			"SEND_IT %d bytes to %d",
			msglen,
			myMsg->toWhom
			);

		rbytes=Send(myMsg->toWhom,
			&myMsg->dataMark,
			workArea,
			msglen,
			MAX_MSG_BUFFER_SIZE);

// reply will be discarded for SG_SEND_NO_REPLY

		if(myToken == SG_SEND_IT)
			replyToSocket(sender,workArea,rbytes);
		}
		break;

	case SG_REPLY_IT:
		{
		SG_REPLY_MSG *myMsg;
		int msglen;
	
		myMsg = (SG_REPLY_MSG *)inArea;
		msglen = myMsg->nbytes-sizeof(int);

		fcLogx(__FILE__, fn,
			globalMask,
			TCL_SURROGATE_MISC,
			"REPLY_IT %d bytes to %d",
			msglen,
			myMsg->toWhom
			);

		if(myMsg->toWhom > 255)
			{
			fcLogx(__FILE__, fn,
				globalMask,
				TCL_SURROGATE_MISC,
				"GOT THAT %d AGAIN",
				myMsg->toWhom
				);
			}
		else
			{	
			Reply(myblockedSenders[myMsg->toWhom], &myMsg->dataMark, msglen);
#if 0
			loadReply(myMsg->toWhom, &myMsg->dataMark, msglen);
			replyMsg(myMsg->toWhom);
#endif
			forgetSender(myMsg->toWhom);
			}
		}
		break;

	case SG_IS_LOGGER_UP:
		{
		SG_IS_LOGGER_UP_MSG *myMsg;
		SG_IS_LOGGER_UP_MSG *replyMsg;
	
		myMsg = (SG_IS_LOGGER_UP_MSG *)&inMsg->dataMark;

		fcLogx(__FILE__, fn,
			globalMask,
			TCL_SURROGATE_MISC,
			"IS_LOGGER_UP logger_fd=%d",
			logger_ID
			);

		replyMsg=(SG_IS_LOGGER_UP_MSG *)workArea;
		replyMsg->rc = logger_ID;
		replyToSocket(sender,workArea,sizeof(SG_IS_LOGGER_UP_MSG));
		}
		break;

	case SG_LOGIT:
		{
		SG_LOGIT_MSG *myMsg;
	
		myMsg = (SG_LOGIT_MSG *)&inMsg->dataMark;

#if 0
printf("%s: LOGIT msg <%s> nbytes=%d\n",
	fn,&myMsg->dataMark,myMsg->nbytes);
#endif

		fcLogx(myMsg->fileName
			, myMsg->funcName
			, myMsg->logMask
			, myMsg->thisMask
			, "%s"
			, &myMsg->dataMark
			);
		}
		break;

	case SG_ACK:
		{
		fcLogx(__FILE__, fn,
			globalMask,
			TCL_SURROGATE_MISC,
			"got SG_ACK"
			);
		}
		break;

	case SG_PING:
		{
		replyToSocket(sender,NULL,0);

		fcLogx(__FILE__, fn,
			globalMask,
			TCL_SURROGATE_MISC,
			"got SG_PING"
			);
		}
		break;

	default:
		replyToSocket(sender,NULL,0);
 		printf("%s: unknown token=%d\n",
			fn,inMsg->token);

		fcLogx(__FILE__, fn,
			globalMask,
			TCL_SURROGATE_MISC,
			"unknown token=%d",
			inMsg->token
			);
		break;
	} // end switch

return(x_it);

} // end hndlSocket
