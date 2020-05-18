/*============================================
	fifoSender.h

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
============================================*/
/* 
 *$Log: fifoSender.h,v $
 *Revision 1.1  2009/01/19 20:14:14  bobfcsoft
 *relocated files
 *
 *Revision 1.2  2007/07/24 21:47:20  bobfcsoft
 *new contact info
 *
 *Revision 1.1.1.1  2005/03/27 11:50:45  paul_c
 *Initial import
 *
 *Revision 1.4  2002/11/22 16:29:33  root
 *2.0rc3
 *
 *Revision 1.3  2002/06/12 18:23:04  root
 *fcipc merge completed
 *
 *Revision 1.2  2000/10/13 14:02:08  root
 *LGPL
 *
 *Revision 1.1  1999/11/04 15:36:15  root
 *Initial revision
 *
 *Revision 1.1  1999/07/13 18:08:18  root
 *Initial revision
 *
 *Revision 1.1  1999/06/29 13:31:05  root
 *Initial revision
 *
 */
#ifndef _FIFO_SENDER_DEFS
#define _FIFO_SENDER_DEFS

#include "simpl.h"

_ALLOC unsigned int globalMask;
#define FIFO_SENDER_MARK		0x00000001
#define FIFO_SENDER_FUNC_IO		0x00000002
#define FIFO_SENDER_MISC		0x00000010

_ALLOC int toPid;
_ALLOC char toName[20];
_ALLOC char myMsg[80];

_ALLOC char inArea[8192];
_ALLOC char outArea[8192];

#endif
