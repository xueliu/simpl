/***********************************************************************

FILE:			surrogate.h

DATE:			02 Aug 22

DESCRIPTION:	This file contains necessary headers, definitions
				and global variables for the tcp surrogate files.

-----------------------------------------------------------------------
    Copyright (C) 2000, 2002, 2005 FCSoftware Inc. 

    This software is in the public domain.
    Permission to use, copy, modify, and distribute this software and its
    documentation for any purpose and without fee is hereby granted, 
    without any conditions or restrictions.
    This software is provided "as is" without express or implied warranty.

    If you discover a bug or add an enhancement here's how to reach us: 

	fcsoft@allstream.net
-----------------------------------------------------------------------
		
REVISIONS:
$Log: surrogate.h,v $
Revision 1.3  2007/01/09 15:33:51  bobfcsoft
fix tcp_x problem

Revision 1.5  2006/04/24 22:04:50  bobfcsoft
added b_port

Revision 1.4  2006/01/10 15:32:22  bobfcsoft
v3.0 changes

Revision 1.3  2005/12/20 15:26:18  bobfcsoft
more 3.0 prerelease code

Revision 1.2  2005/11/24 21:46:57  bobfcsoft
2nd 3.0 prerelease

***********************************************************************/

#ifndef _SURROGATE_H
#define _SURROGATE_H

// standard headers
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

// simpl headers
#include <simpl.h>
#include <simplLibProto.h>
#include <simplDefs.h>
#include <simplmiscProto.h>
#include <simplProtocols.h>
#include <simplNames.h>
#include <surMsgs.h>

// definitions
#define A_PORT						8002
#define INT_WIDTH_32				4 // the size of a 32-bit int
#define INT_WIDTH_64				8 // the size of a 64-bit int
#define	NAMELOC_TIMEOUT				60 // (sec) fail time on nameloc request 
#define	KEEP_ALIVE_TIMEOUT			10 // (sec) 0 implies no keep alive
#define	KEEP_ALIVE_FAIL_LIMIT		3 // allowed keep alive failures
#define	KEEP_ALIVE_RESPONSE_TIMEOUT	3 // acknowledge wait time (sec)
#define	RESPONSE_TIMEOUT			10 // acknowledge wait time (sec)
#define MAX_PENDING_CONNECTIONS		25
#define MAX_SURROGATES				50

// function prototypes in surrogateInit.c
int initialize(int, char **);

// function prototypes in surrogate_cparent.c
void surrogate_cparent(void);
pid_t forkSurrogate(void);
int sendSurrogateMsg(int, pid_t);

// function prototypes in surrogate_connector.c
int surrogate_connector(void);
int surrogate_r(void);
int surrogate_st(char *);
int hndlRemoteNameLocate(pid_t, SIMPL_REC *);
int hndlSendMsg(void);
int hndlRemoteMsg(int *);
int sendKeepAliveMsg(int *);
void hndlClose(void);
int hndlProxy(int);

// function prototypes in surrogate_listener.c
void surrogate_listener(void);

// function prototypes in surrogate_s.c
void surrogate_s(void);
int surrogate_rt();
int locateLocalName(SIMPL_REC *);
int hndlMessage(int, int *, int *);
int hndlReply(int);
void errorReply(void);

// function prototypes in surrogateUtils.c
int initRsock(unsigned);
int initSsock(unsigned, char *);
int surRead(void *, unsigned);
int surWrite(void *, unsigned);
void replyFailure(char *);
void killZombies(void);
int adjustMemory(int);
int nameLocateReply(int);
int nameAttachReply(int);

// globals
_ALLOC char *sender;
_ALLOC unsigned a_port;  // listening port
_ALLOC unsigned b_port;	 // client port
_ALLOC int intWidth;
_ALLOC int namelocTimeout;
_ALLOC int kaTimeout;
_ALLOC int kaResponseTimeout;
_ALLOC int mytimeout;
_ALLOC struct sockaddr_in rserver;
_ALLOC char *memArea;
_ALLOC char *amemArea;
_ALLOC int memSize;
_ALLOC int amemSize;
_ALLOC int surSock;
_ALLOC int receiving;
_ALLOC SIMPL_REC senderInfo;
_ALLOC SUR_NAME_ATTACH_MSG clone;


#ifdef BUFFERED
	_ALLOC int dupSock;
	_ALLOC FILE *rFp;
	_ALLOC FILE *wFp;
#endif

#ifdef _SURROGATE_TCP_PRIMARY
	_ALLOC socklen_t sock_len = sizeof(struct sockaddr_in);
#else
	_ALLOC socklen_t sock_len;
#endif

#endif
