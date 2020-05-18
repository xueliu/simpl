/*************************************************************
FILE:		surroTestMsgs.h

DESCRIPTION:	
This file contains test message templates for tclSurrogate modules.

AUTHOR:		R.D. Findlay

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
$Log: surroTestMsgs.h,v $
Revision 1.1  2009/01/19 20:14:14  bobfcsoft
relocated files

Revision 1.2  2007/07/24 21:47:20  bobfcsoft
new contact info

Revision 1.1.1.1  2005/03/27 11:50:45  paul_c
Initial import

Revision 1.2  2000/10/13 14:03:32  root
LGPL

Revision 1.1  1999/11/04 15:37:30  root
Initial revision

=======================================================

*************************************************************/

#ifndef _SURROTEST_MSGS_DEF
#define _SURROTEST_MSGS_DEF

#include "standardTypes.h"

/*=======================================
	 message tokens
=======================================*/
typedef enum
	{
	SURRO_TEST=0xfc01,
	MAX_NUM_TEST_TOKENS
	}SURRO_TEST_TOKEN;

/*============================================
	message templates
============================================*/
typedef struct  	// used for testing purposes
	{
	UINT16 token;		// for SURRO_TEST
	char msg[80];
	}SURRO_TEST_MSG;
	
#endif
