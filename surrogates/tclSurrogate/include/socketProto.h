/*===================================================
FILE:		socketProto.h

Description:
This file contains the master list of prototypes for
functions.

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
===================================================*/
/*
 *$Log: socketProto.h,v $
 *Revision 1.1  2009/01/19 20:14:14  bobfcsoft
 *relocated files
 *
 *Revision 1.3  2007/07/24 21:24:41  bobfcsoft
 *new contact info
 *
 *Revision 1.2  2007/03/07 17:51:18  bobfcsoft
 *trusted network change
 *
 *Revision 1.1.1.1  2005/03/27 11:50:45  paul_c
 *Initial import
 *
 *Revision 1.4  2003/09/11 15:43:32  root
 *some new functions to support MSG_WAITALL
 *
 *Revision 1.3  2002/06/12 18:23:38  root
 *fcipc merge completed
 *added ack timer stuff
 *
 *Revision 1.2  2000/10/13 14:02:43  root
 *LGPL
 *
 *Revision 1.1  1999/11/04 15:36:36  root
 *Initial revision
 *
 *Revision 1.2  1999/08/10 19:06:03  root
 *cleanup
 *
 *Revision 1.1  1999/07/13 18:08:31  root
 *Initial revision
 *
 *Revision 1.1  1999/06/29 13:31:34  root
 *Initial revision
 *
 */
/*=================================================*/
#ifndef _SOCKET_PROTO
#define _SOCKET_PROTO

int replyToSocket(int, char *, int);
int receiveFromSocket(int, char *);
int readBytesFromSocket(int, int, char *);
#ifndef MSG_WAITALL
int recvWAITALL(int, char *, int);
#endif
int sendToSocket( int, char *, int, char *, int);
int relayToSocket( int, char *, int);
int connectSocket(char *, int); 
int acceptSocket(int);
int attachSocket(int);
void detachSocket(int);
char *getMyHost();
void killZombies();
int setWaitingForAck();
int clrWaitingForAck();
int isWaitingForAck();
int slayProcess(char *, int);

#endif
