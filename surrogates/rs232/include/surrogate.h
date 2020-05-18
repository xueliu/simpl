/***********************************************************************

FILE:			surrogate.h

DATE:			07 July 23

DESCRIPTION:	This file contains necessary headers, definitions
				and global variables for the tcp surrogate files.

-----------------------------------------------------------------------
    Copyright (C) 2005 FCSoftware Inc. 

    This software is in the public domain.
    Permission to use, copy, modify, and distribute this software and its
    documentation for any purpose and without fee is hereby granted, 
    without any conditions or restrictions.
    This software is provided "as is" without express or implied warranty.

    If you discover a bug or add an enhancement contact us on the
    SIMPL project mailing list.
-----------------------------------------------------------------------
REVISIONS:
$Log: surrogate.h,v $
Revision 1.3  2010/01/30 11:12:52  bobfcsoft
NO_ZOMBIE default

Revision 1.2  2010/01/29 20:38:27  johnfcsoft
added signal.h for zombies

Revision 1.1  2008/04/21 17:15:58  johnfcsoft
initial


***********************************************************************/

#ifndef _SURROGATE_H
#define _SURROGATE_H

// standard headers
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#ifdef ZOMBIE
#include <sys/wait.h>
#endif

// simpl headers
#include <simpl.h>
#include <simplLibProto.h>
#include <simplDefs.h>
#include <simplmiscProto.h>
#include <simplProtocols.h>
#include <simplNames.h>
#include <surMsgs.h>

// definitions
#define INT_WIDTH_32				4 // the size of a 32-bit int
#define INT_WIDTH_64				8 // the size of a 64-bit int
#define	NAMELOC_TIMEOUT				60 // (sec) fail time on nameloc request 
#define	KEEP_ALIVE_TIMEOUT			10 // (sec) 0 implies no keep alive
#define	KEEP_ALIVE_FAIL_LIMIT		3 // allowed keep alive failures
#define	KEEP_ALIVE_RESPONSE_TIMEOUT	3 // acknowledge wait time (sec)
#define	RESPONSE_TIMEOUT			10 // acknowledge wait time (sec)
#define MAX_SURROGATES				50

// for dynamic memory allocations
typedef enum
	{
	IN,
	OUT
	} MSG_TYPE;

// function prototypes in surrogateInit.c
int initialize(int, char **);

// function prototypes in surrogate_R.c
void surrogate_R(void);
pid_t forkSurrogate(void);
int sendSurrogateMsg(int, pid_t);

// function prototypes in surrogate_r.c
void surrogate_r(void);
int hndlRemoteNameLocate(SIMPL_REC *);
int hndlMsg(int *);
int sendKeepAliveMsg(int *);
void hndlClose(void);
int hndlProxy(int);
void replyFailure(char *);

// function prototypes in surrogate_S.c
void surrogate_S(void);

// function prototypes in surrogate_s.c
void surrogate_s(void);
int locateLocalName(SIMPL_REC *);
int nameLocateReply(int);
int hndlMessage(int, int *, int *);
int hndlReply(int);
void errorReply(void);

// function prototypes in surrogateUtils.c
int checkMemory(int, int);
void killZombies(void);

// globals
_ALLOC int surRpid;
_ALLOC int surSpid;
_ALLOC int serialWriter;
_ALLOC int intWidth;
_ALLOC int namelocTimeout;
_ALLOC int kaTimeout;
_ALLOC int kaResponseTimeout;
_ALLOC char *inMsgArea;
_ALLOC int inMsgSize;
_ALLOC char *outMsgArea;
_ALLOC int outMsgSize;

#endif
