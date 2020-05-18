/***********************************************************************

FILE:			surrogateUtils.c

DATE:			05 Oct 31 

DESCRIPTION:	This file contains useful utility functions used by the
				various surrogate*.c files.

AUTHOR:			FC Software Inc.
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
$Log: surrogateUtils.c,v $
Revision 1.4  2010/01/31 23:25:24  bobfcsoft
ifdef killZombie function

Revision 1.3  2007/07/23 21:17:27  bobfcsoft
-11 should read -1 at line 380

Revision 1.2  2007/01/09 15:30:16  bobfcsoft
restore from v3.1 + runaway patch

Revision 1.7  2006/12/22 15:46:47  bobfcsoft
fixes for runaway on socket closure

Revision 1.6  2006/04/24 22:16:25  bobfcsoft
added nbytes to LOCATE_REPLY header

Revision 1.5  2006/01/26 02:15:17  bobfcsoft
v3.x enhancements

Revision 1.4  2006/01/10 15:32:22  bobfcsoft
v3.0 changes

***********************************************************************/

// application header
#define _ALLOC extern
#include "surrogate.h"
#undef _ALLOC

/**********************************************************************
FUNCTION:	initRsock(unsigned)

PURPOSE:	Create and initialize a socket to be used for receiving.

RETURNS:	int: 0=success, -1=failure
**********************************************************************/	

int initRsock(unsigned rport)
{
int rsock;
const int i = 1;
const static char *fn = "initRsock";
// rserver is global

/*
create an unitialized (not connected to anything) TCP/IP (PF_INET)
stream protocol socket (SOCK_STREAM)
*/
rsock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
if (rsock < 0)
	{
	_simpl_log("%s: socket creation error-%s\n", fn, strerror(errno));
	return(-1);
	}

// let the kernel reuse this socket address
setsockopt(rsock, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(i));

/*
At this point you may wish to augment socket behaviour with further calls to
setsockopt() and/or getsockopt().
*/

// tell the system which address to use for the socket
// address family
rserver.sin_family = AF_INET;
// port number converted network format if necessary
rserver.sin_port = htons(rport); 
memset(&rserver.sin_addr, 0, sizeof(rserver.sin_addr));

// name (assign an address) to this socket w.r.t this port
if (bind(rsock, (struct sockaddr *) &rserver, sock_len) == -1)
	{
	_simpl_log("%s: bind error-%s\n", fn, strerror(errno));
	return(-1);
	}

// willing to allow connections to the socket
if (listen(rsock, MAX_PENDING_CONNECTIONS) == -1)
	{
	_simpl_log("%s: listen error-%s\n", fn, strerror(errno));
	return(-1);
	}

// got this far without mishap
return(rsock);
}

/**********************************************************************
FUNCTION:	initSsock(unsigned, char *)

PURPOSE:	Create and initialize a socket to be used for sending.

RETURNS:	int: 0=success, -1=failure
**********************************************************************/	

int initSsock(unsigned sport, char *hostName)
{
int ssock;
const int i = 1;
struct hostent *hp;
struct  sockaddr_in sserver;
const static char *fn = "initSsock";

/*
create an unitialized (not connected to anything) TCP/IP (PF_INET)
stream protocol socket (SOCK_STREAM)
*/
ssock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
if (ssock < 0)
	{
	// bad result
	_simpl_log("%s: socket creation error-%s\n", fn, strerror(errno));
	return(-1);
	}

// let the kernel reuse this socket address
setsockopt(ssock, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(i));

/*
At this point you may wish to augment socket behaviour with further calls to
setsockopt() and/or getsockopt().
*/

// tell the system which address to use for the socket
// address family
sserver.sin_family = AF_INET;

// port number converted network format if necessary
sserver.sin_port = htons(sport);

// try to convert as dotted decimal address, if that fails assume it's a hostname
//sserver.sin_addr.s_addr = inet_addr(hostName);
//if (sserver.sin_addr.s_addr == INADDR_NONE)
if ( !inet_aton(hostName, &(sserver.sin_addr)) )
	{
	// read the /etc/hosts file for the connection point
	hp = gethostbyname(hostName);
	if (hp == NULL)
		{
		// bad result
		_simpl_log("%s: host file problem-%s or no host called %s\n",
						fn,
						strerror(h_errno),
						hostName);
		return(-1);
		}

	memcpy(&sserver.sin_addr, hp->h_addr, hp->h_length);
	}
else
	{
	}

// try to connect to the other node's surrogate_S
if (connect(ssock, (struct sockaddr *) &sserver, sizeof(sserver)) < 0)
	{
	_simpl_log("%s: connect error-%s\n", fn, strerror(errno));
	return(-1);
	}

return(ssock);
}

/**********************************************************************
FUNCTION:	surRead(void *, unsigned)

PURPOSE:	Read a stream for a well-defined number of bytes.

RETURNS:	int
**********************************************************************/	

int surRead(void *ptr, unsigned nBytes)
{
// surSock is global
// rFp is global

#ifdef BUFFERED
	if (fread(ptr, nBytes, 1, rFp) != 1)
		{
		return(-1);
		}
	return(0);
#else
	int numLeft = nBytes;
	int numRead;

	while (numLeft > 0)
		{
		numRead = read(surSock, ptr, numLeft);

		if (numRead < 0)
			{
			return(numRead);
			}
		else if (numRead == 0)
			{
			if((nBytes - numLeft) == 0)
				{
				errno=EIO;
				return(-1);
				} 
			else
				break;
			}

		numLeft -= numRead;
	
		ptr += numRead;
		}

	return(nBytes - numLeft);
#endif
}

/**********************************************************************
FUNCTION:	surWrite(void *, unsigned)

PURPOSE:	Write a stream for a well-defined number of bytes.

RETURNS:	int
**********************************************************************/	

int surWrite(void *ptr, unsigned nBytes)
{
// surSock is global
// wFp is global

#ifdef BUFFERED
	if (fwrite(ptr, nBytes, 1, wFp) != 1)
		{
		return(-1);
		}
	fflush(wFp);
#else
	if (write(surSock, ptr, nBytes) != nBytes)
		{
		return(-1);
		}
#endif

return(0);
}

/**********************************************************************
FUNCTION:	int adjustMemory(int)

PURPOSE:	Adjust the global dynamic memory for upward growth. 

RETURNS:	int	
**********************************************************************/	

int adjustMemory(int nbytes)
{
// memArea is global
// memSize is global
// amemArea is global
// amemSize is global
const static char *me = "adjustMemory";

#ifdef SUR_CHR	/********** char message **********/
	const static int size = sizeof(SUR_MSG_CHR_HDR);
#else			/********** binary message **********/
	const static int size = sizeof(SUR_MSG_HDR);
#endif 

// realloc memory
memArea = realloc(memArea, nbytes + size);
if (!memArea)
	{ 
	_simpl_log("%s: memory allocation error-%s\n", me, strerror(errno));
	return(-1);
	}

// reset global memory stuff
amemArea = memArea + size;
memSize = nbytes + size;
amemSize = nbytes;

return(0);
}

/**********************************************************************
FUNCTION:	replyFailure(char *)

PURPOSE:	Failure reply function used by Receive surrogates to advise
			a local sender of communication errors resulting in a
			Send() failure.

RETURNS:	void
**********************************************************************/	

void replyFailure(char *caller)
{
static SUR_NAME_LOCATE_REPLY reply = {{0, 0}, -1,};

Reply(caller, &reply, sizeof(SUR_NAME_LOCATE_REPLY));
}

#ifdef ZOMBIE
/**********************************************************************
FUNCTION:	killZombies(void)

PURPOSE:	Forking parents make use of this call to release zombie
			children by retrieving their exit status.

			Also helps to keep the surrogate pid array current.

RETURNS:	void
**********************************************************************/	

void killZombies()
{
register int i;
pid_t pid;

for (i = 0; i < MAX_SURROGATES; i++)
	{
	pid = waitpid(-1, NULL, WNOHANG | WUNTRACED);
	if (pid <= 0)
		{
		break;
		}
	}
}
#endif

/**********************************************************************
FUNCTION:	nameLocateReply(int)

PURPOSE:	This function reports the results of a remote name_locate()
			to the calling receiver surrogate.

RETURNS:	void
**********************************************************************/	

int nameLocateReply(int result)
{
// memArea is global
// wFp is global
const static char *fn = "nameLocateReply";

#ifdef SUR_CHR	/********** char message **********/
	const static int size = sizeof(SUR_NAME_LOCATE_CHR_REPLY);
	SUR_NAME_LOCATE_CHR_REPLY *reply;
#else			/********** binary message **********/
	const static int size = sizeof(SUR_NAME_LOCATE_REPLY);
	SUR_NAME_LOCATE_REPLY *reply;
#endif

// build the reply message
#ifdef SUR_CHR	/********** char message **********/
	reply = (SUR_NAME_LOCATE_CHR_REPLY *)memArea;
	btosSI(SUR_NAME_LOCATE, reply->hdr.token, intWidth);
	btosSI(result, reply->result, intWidth);
#else			/********** binary message **********/
	reply = (SUR_NAME_LOCATE_REPLY *)memArea;
	reply->hdr.token = SUR_NAME_LOCATE;
	reply->hdr.nbytes = sizeof(SUR_NAME_LOCATE_REPLY) - sizeof(SUR_MSG_HDR);
	reply->result = result;
#endif
	
if (surWrite(memArea, size) == -1)
	{
	_simpl_log("%s: write error-%s\n", fn, strerror(errno));
	return(-1);
	}

return(0);
}
