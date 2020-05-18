/*************************************************************
FILE:		tclSurrogate.h

DESCRIPTION:	
This file is the main include file for tclSurrogate

AUTHOR:			R.D. Findlay

-----------------------------------------------------------------------
    Copyright (C) 1999, 2002, 2007 FCSoftware Inc. 

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
$Log: tclSurrogate.h,v $
Revision 1.1  2009/01/19 20:14:14  bobfcsoft
relocated files

Revision 1.4  2007/07/24 21:26:47  bobfcsoft
new contact info

Revision 1.3  2007/03/07 17:51:18  bobfcsoft
trusted network change

Revision 1.2  2006/04/24 22:08:47  bobfcsoft
added TRACK mask

Revision 1.1.1.1  2005/03/27 11:50:45  paul_c
Initial import

Revision 1.7  2002/11/22 16:30:30  root
2.0rc3

Revision 1.6  2002/11/19 15:04:32  root
2.0rc1

Revision 1.5  2002/06/12 18:25:09  root
fcipc merge completed
added maxAckTime for ack timer

Revision 1.4  2001/12/15 12:30:46  root
added blocked senders array

Revision 1.3  2000/10/13 14:04:16  root
LGPL
expanded to 2k messages

Revision 1.2  1999/12/30 12:13:17  root
introduced childsName and childsSlot as globals

Revision 1.1  1999/11/04 15:37:46  root
Initial revision

=======================================================

*************************************************************/

#ifndef _TCL_SURROGATE_DEF
#define _TCL_SURROGATE_DEF

#include "simpl.h"
#include "loggerVars.h"

#define	MAX_MSG_BUFFER_SIZE		2048

_ALLOC unsigned int globalMask;
#define TCL_SURROGATE_MARK		0x00000001
#define TCL_SURROGATE_FUNC_IO		0x00000002
#define TCL_SURROGATE_MISC		0x00000010
#define TCL_SURROGATE_TRACK		0x00000020

_ALLOC char * myblockedSenders[32];  // list of blocked senders
_ALLOC pid_t myProxy;
_ALLOC int fd;
_ALLOC int my_fds[3];
_ALLOC fd_set watchset;
_ALLOC fd_set inset;
_ALLOC int maxfd;
_ALLOC char myPort[40];
_ALLOC int mySocket;
_ALLOC int socket_fd;
_ALLOC int mysender;
_ALLOC int isThisChild;
_ALLOC int myIndex;
_ALLOC char me[30];
_ALLOC int maxAckTime;
_ALLOC int trustedNetworkFlag;

_ALLOC int parents_id;
_ALLOC char parentsName[20];
_ALLOC char childsName[20];
_ALLOC int childsSlot;
_ALLOC char logger_name[20];

_ALLOC char inArea[MAX_MSG_BUFFER_SIZE];
_ALLOC char outArea[MAX_MSG_BUFFER_SIZE];
_ALLOC char workArea[MAX_MSG_BUFFER_SIZE];

#endif
