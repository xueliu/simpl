
/*************************************************************

FILE:		tclSurroProto.h

DESCRIPTION:	
This file contains prototypes for receiver example.

AUTHOR:		R.D. Findlay

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
$Log: tclSurroProto.h,v $
Revision 1.1  2009/01/19 20:14:14  bobfcsoft
relocated files

Revision 1.2  2007/07/24 21:47:20  bobfcsoft
new contact info

Revision 1.1.1.1  2005/03/27 11:50:45  paul_c
Initial import

Revision 1.5  2002/11/22 16:30:19  root
2.0rc3

Revision 1.4  2002/11/19 15:05:29  root
2.0rc1

Revision 1.3  2001/12/15 12:30:13  root
prototypes for rememberSender, forgetSender, unblockSenders

Revision 1.2  2000/10/13 14:03:53  root
LGPL

Revision 1.1  1999/11/04 15:37:39  root
Initial revision

=======================================================

*************************************************************/

#ifndef _TCLSURRO_PROTO_DEF
#define _TCLSURRO_PROTO_DEF

#include "simplLibProto.h"  // for ReplyError()

void initialize(int, char **);
void hndlWorld();
void myUsage();
int myExit();

void initChild(int);
int hndlSocket();
int hndlChildFifo();
int hndlParentFifo();
int doParent();
int doChild();

int rememberSender(char *);
int forgetSender(int);
int unblockSenders();

#endif
