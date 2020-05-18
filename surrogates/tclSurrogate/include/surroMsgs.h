/*************************************************************
FILE:		surroMsgs.h

DESCRIPTION:	
This file contains messaging templates and tokens for
Tcl/Tk surrogate process.

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
$Log: surroMsgs.h,v $
Revision 1.1  2009/01/19 20:14:14  bobfcsoft
relocated files

Revision 1.2  2007/07/24 21:47:20  bobfcsoft
new contact info

Revision 1.1.1.1  2005/03/27 11:50:45  paul_c
Initial import

Revision 1.6  2002/11/22 16:29:58  root
2.0rc3

Revision 1.5  2002/06/12 18:24:40  root
added ACK and PING tokens

Revision 1.4  2000/10/13 14:03:16  root
LGPL

Revision 1.3  2000/04/17 15:11:44  root
added SEND_NO_REPLY

Revision 1.2  1999/12/30 12:13:48  root
simplified message structure

Revision 1.1  1999/11/04 15:37:19  root
Initial revision

Revision 1.1  1999/07/13 18:08:42  root
Initial revision

=======================================================

*************************************************************/

#ifndef _SURRO_MSGS_DEF
#define _SURRO_MSGS_DEF

#include "standardTypes.h"

typedef enum
	{
	SG_NAME_ATTACH,
	SG_NAME_DETACH,
	SG_NAME_LOCATE,
	SG_SEND_IT,
	SG_REPLY_IT,
	SG_RELAY_IT,
	SG_IS_LOGGER_UP,
	SG_LOGIT,
	SG_SEND_NO_REPLY,
	SG_ACK,
	SG_PING,
	MAX_VC_TOKENS
	}SG_TOKEN;

typedef struct
	{
	UINT16 token;	// SG_SEND_IT, SG_SEND_NO_REPLY
	UINT16 nbytes;
	int toWhom;
	char dataMark;	// marks start of data
	}SG_SEND_MSG;

typedef struct
	{
	UINT16 token;	// SG_REPLY_IT
	UINT16 nbytes;
	int toWhom;
	char dataMark;	// marks start of data
	}SG_REPLY_MSG;

typedef struct
	{
	UINT16 token;	// SG_RELAY_IT
	UINT16 nbytes;
	int fromWhom;
	char dataMark;	// marks start of data
	}SG_RELAY_MSG;

typedef struct
	{
	UINT16 token;	// SG_NAME_DETACH
	UINT16 nbytes;
	}SG_NAME_DETACH_MSG;

typedef struct
	{
	UINT16 token;	// SG_ACK, SG_PING
	UINT16 nbytes;
	}SG_ACK_MSG;

/*=========================================
	structures at dataMark
=========================================*/
typedef struct
	{
	int rc;
	}SG_GENERIC_ANSWER;

typedef struct
	{
	char myName[20];
	int myPid;
	int mySlot;
	}SG_NAME_ATTACH_MSG;

typedef struct
	{
	char thisName[20];
	int rc;
	}SG_NAME_LOCATE_MSG;

typedef struct
	{
	char loggerName[20];
	int rc;
	}SG_IS_LOGGER_UP_MSG;

typedef struct
	{
	char fileName[20];
	char funcName[20];
	UINT32 thisMask;
	UINT32 logMask;
	char dataMark;
	}SG_LOGIT_MSG;

#endif
