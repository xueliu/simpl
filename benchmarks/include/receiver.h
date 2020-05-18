/*============================================
	receiver.h

-----------------------------------------------------------------------
    Copyright (C) 1998 FCSoftware Inc. 

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
============================================*/
/*
 *$Log: receiver.h,v $
 *Revision 1.3  2011/03/23 16:39:57  bobfcsoft
 *bigger buffers
 *
 *Revision 1.2  2007/07/24 20:10:23  bobfcsoft
 *new contact info
 *
 *Revision 1.1.1.1  2005/03/27 11:50:37  paul_c
 *Initial import
 *
 *Revision 1.7  2002/05/29 15:27:00  root
 *enhanced with _SIMPLFCIPC compatibility
 *
 *Revision 1.6  2001/07/05 14:40:00  root
 *added _SIMPLIPC case
 *
 *Revision 1.5  2001/01/29 15:10:46  root
 *using fcipc.h
 *
 *Revision 1.4  2000/10/13 13:49:53  root
 *LGPL
 *
 *Revision 1.3  2000/04/17 14:55:42  root
 *enhanced for SRR, SIPC and QNX
 *
 *Revision 1.2  1999/02/04 15:45:44  root
 **** empty log message ***
 *
 */
#ifndef _RECEIVER_DEFS
#define _RECEIVER_DEFS

#ifdef _FIFO
#include "fcipc.h"
#endif

#ifdef _SIMPLFCIPC
#include "simpl.h"
#endif

#ifdef _SIMPLIPC
#include "simpl.h"
#endif

#ifdef _SRR
#include "srrWrapper.h"
#endif

#ifdef _SRR
_ALLOC nameid_t nameid;
#endif

_ALLOC char inArea[16384];
_ALLOC char outArea[16384];

#endif
