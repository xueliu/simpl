/*************************************************************
FILE:		socketSender.h

DESCRIPTION:	
This file is the main include file for lcTalker

AUTHOR:			R.D. Findlay

-----------------------------------------------------------------------
    Copyright (C) 1999, 2002, 2007 FCSoftware Inc. 

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

    If you discover a bug or add an enhancement contact us on the
    SIMPL project mailing list. 

-----------------------------------------------------------------------
Revision history:
=======================================================
$Log: socketSender.h,v $
Revision 1.1  2009/01/19 20:14:14  bobfcsoft
relocated files

Revision 1.2  2007/07/24 21:47:20  bobfcsoft
new contact info

Revision 1.1.1.1  2005/03/27 11:50:45  paul_c
Initial import

Revision 1.3  2002/06/12 18:24:26  root
fcipc merge completed

Revision 1.2  2000/10/13 14:03:00  root
LGPL

Revision 1.1  1999/11/04 15:36:57  root
Initial revision

Revision 1.3  1999/08/10 19:07:08  root
cleanup

Revision 1.2  1999/08/03 22:35:45  root
added logger stuff

Revision 1.1  1999/07/13 18:08:37  root
Initial revision

Revision 1.1  1999/06/29 13:32:12  root
Initial revision

Revision 1.1  1999/04/09 21:17:42  root
Initial revision

=======================================================

*************************************************************/

#ifndef _SOCKET_SENDER_DEF
#define _SOCKET_SENDER_DEF

#include <termios.h>
#include "loggerVars.h"

#define	MAX_MSG_BUFFER_SIZE		1024

_ALLOC unsigned int globalMask;
#define SOCKET_SENDER_MARK		0x00000001

_ALLOC struct termios sots;
_ALLOC pid_t myProxy;
_ALLOC int fd;
_ALLOC int maxfd;
_ALLOC int my_fds[2];
_ALLOC fd_set watchset;
_ALLOC fd_set inset;
_ALLOC pid_t agentPid;
_ALLOC pid_t senderPid;
_ALLOC int socketConnected;
_ALLOC int mySocket;
_ALLOC int testMode;

_ALLOC int myPort;

_ALLOC char inArea[MAX_MSG_BUFFER_SIZE];
_ALLOC char outArea[MAX_MSG_BUFFER_SIZE];

#endif
