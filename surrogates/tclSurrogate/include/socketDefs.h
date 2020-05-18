/*************************************************************
	FILE:		socketDefs.h

	DESCRIPTION:	
	This file contains major definitions for TCPIP messaging.

	AUTHOR:			R.D. Findlay

-----------------------------------------------------------------------
    Copyright (C) 1999, 2007 FCSoftware Inc. 

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
$Log: socketDefs.h,v $
Revision 1.2  2010/02/19 16:29:16  bobfcsoft
added TRACK trace bit

Revision 1.1  2009/01/19 20:14:14  bobfcsoft
relocated files

Revision 1.2  2007/07/24 21:47:20  bobfcsoft
new contact info

Revision 1.1.1.1  2005/03/27 11:50:45  paul_c
Initial import

Revision 1.2  2000/10/13 14:02:27  root
LGPL

Revision 1.1  1999/11/04 15:35:52  root
Initial revision

Revision 1.3  1999/08/23 11:13:08  root
added log mask stuff

Revision 1.2  1999/08/10 19:05:17  root
cleanup

Revision 1.1  1999/07/13 18:08:25  root
Initial revision

Revision 1.2  1999/06/30 19:42:09  root
*** empty log message ***

Revision 1.1  1999/06/29 13:39:16  root
Initial revision

=======================================================

*************************************************************/
#ifndef _SOCKET_DEFS_DEF
#define _SOCKET_DEFS_DEF

#include "standardTypes.h"

#define	MAX_MSG_BUFFER_SIZE		4096

_ALLOC unsigned int globalMask;
#define TCL_SURROGATE_MARK		0x00000001
#define TCL_SURROGATE_FUNC_IO		0x00000002
#define TCL_SURROGATE_MISC		0x00000010
#define TCL_SURROGATE_TRACK		0x00000020

#if 0
typedef struct
	{
	UINT16 token;
	UINT16 numBytes;
	char data[MAX_MSG_BUFFER_SIZE];
	}IPC_MESSAGE;
#endif


_ALLOC char inArea[MAX_MSG_BUFFER_SIZE];
_ALLOC char outArea[MAX_MSG_BUFFER_SIZE];

#endif
