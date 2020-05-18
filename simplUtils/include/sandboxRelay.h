/*************************************************************
FILE:	sandboxRelay.h	

DESCRIPTION:	
This file contains globals for sandboxRelay app.

AUTHOR:			R.D. Findlay

-----------------------------------------------------------------------
    Copyright (C) 2007 SIMPL project. 

    This software is in the public domain.
    Permission to use, copy, modify, and distribute this software and its
    documentation for any purpose and without fee is hereby granted, 
    without any conditions or restrictions.
    This software is provided "as is" without express or implied warranty.

    If you discover a bug or add an enhancement contact the SIMPL 
    project mailing list. 

-----------------------------------------------------------------------
Revision history:
=======================================================
$Log: sandboxRelay.h,v $
Revision 1.1  2007/07/03 18:28:04  bobfcsoft
added sandboxRelay

=======================================================

*************************************************************/

#ifndef _RELAY_DEFS
#define _RELAY_DEFS

#define MAX_MSG_SIZE	8192

#include "simpl.h"
#include "loggerVars.h"

// some parameters to enable the trace logger
// fclogger
_ALLOC unsigned int globalMask;
#define TRACE_MARK		0x00000001
#define TRACE_FUNC_IO		0x00000002
#define TRACE_MISC		0x00000010

_ALLOC char recv_name[20];
_ALLOC int recvID;
_ALLOC char otherFifoPath[128];
_ALLOC int sb_fd;

_ALLOC char inArea[MAX_MSG_SIZE];
_ALLOC char outArea[MAX_MSG_SIZE];

#endif
