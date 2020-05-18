/*============================================
	sender.h

-----------------------------------------------------------------------
    Copyright (C) 1998 FCSoftware Inc. 

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
 *$Log: sender.h,v $
 *Revision 1.3  2011/03/23 16:39:57  bobfcsoft
 *bigger buffers
 *
 *Revision 1.2  2007/07/24 20:10:23  bobfcsoft
 *new contact info
 *
 *Revision 1.1.1.1  2005/03/27 11:50:37  paul_c
 *Initial import
 *
 *Revision 1.8  2002/11/22 16:24:50  root
 *2.0rc3
 *
 *Revision 1.7  2002/05/29 15:26:38  root
 *enhanced with _SIMPLFCIPC compatibility
 *
 *Revision 1.6  2001/07/05 14:39:37  root
 *added _SIMPLIPC define
 *
 *Revision 1.5  2001/01/29 15:09:33  root
 *using fcipc.h header
 *
 *Revision 1.4  2000/10/13 13:49:38  root
 *LGPL
 *
 *Revision 1.3  2000/04/17 14:55:59  root
 *enhanced for SRR, SIPC and QNX
 *
 *Revision 1.2  1999/02/04 15:46:27  root
 **** empty log message ***
 *
 */
#ifndef _SENDER_DEFS
#define _SENDER_DEFS

#ifdef _FIFO
#include "fcipc.h"   // all public structures
_ALLOC FCID toPid;
#endif

#ifdef _SIMPLFCIPC
#include "simpl.h"   // all public structures

_ALLOC FCID toPid;
#endif


#ifdef _SIMPLIPC
#include "simpl.h"

_ALLOC int toPid;
#endif

#ifdef _SRR
#include "srrWrapper.h"

_ALLOC int toPid;
_ALLOC nameid_t nameid;
#endif

#ifdef _SIPC
_ALLOC int toPid;
#endif

_ALLOC char inArea[16384];
_ALLOC char outArea[16384];

#endif
