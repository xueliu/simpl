/***********************************************************************

FILE:			surrogate_s.c

DATE:			05 Oct 31

DESCRIPTION:	This program runs as a fork from surrogate_S.c
 
NOTES:			surrogate_s receives the following messages:
				1. SUR_SEND (from remote surrogate_r)
				2. SUR_PROXY (from remote surrogate_r)
				2. SUR_CLOSE (from remote surrogate_r)
				3. SUR_ALIVE (from remote surrogate_r)

				surrogate_s sends the following messages:
				1. SUR_ALIVE (to remote surrogate_r)
				2. messages intended for the local receiver.
				
-----------------------------------------------------------------------
    Copyright (C) 2005 FCSoftware Inc. 

    This software is in the public domain.
    Permission to use, copy, modify, and distribute this software and its
    documentation for any purpose and without fee is hereby granted, 
    without any conditions or restrictions.
    This software is provided "as is" without express or implied warranty.

    If you discover a bug or add an enhancement here's how to reach us: 

	fcsoft@allstream.net
-----------------------------------------------------------------------

REVISIONS:
$Log: surrogate_ss.c,v $
Revision 1.1  2011/03/23 17:53:20  bobfcsoft
new file replaces surrogate_s.c

Revision 1.1  2007/01/09 15:28:11  bobfcsoft
restore from v3.1

Revision 1.5  2006/04/24 22:19:19  bobfcsoft
add nbytes field to headers

Revision 1.4  2006/01/10 15:32:22  bobfcsoft
v3.0 changes

***********************************************************************/

// application header
#define _ALLOC extern
#include "surrogate.h"
#undef _ALLOC

/**********************************************************************
FUNCTION:	surrogate_s(int)

PURPOSE:	Handle a remote name locate request and act as a conduit
			by receiving tcp messages from a remote surrogate receiver
			and sending them on to the actual receiver.

RETURNS:	nothing, it is a forked process and never returns.	

PICTURE:

	|			<---	memSize		--->			|
	|		|	<---	amemSize	--->			|
	|  hdr	|			message						|
	^		^
	memArea amemArea

memSize is the total amount of allocated memory in bytes
amemSize is the amount of memory availbale for the actual messages
hdr is the amount of memory required for the protocol
memArea is a pointer to the start of memArea
amemArea is a pointer to memArea + sizeof hdr
**********************************************************************/	

void surrogate_s()
{
// kaTimeout is global
// kaHostName is global
// a_port is global
// surSock is global
// dupSock is global
const char *fn = "surrogate_s";
int fds[2];
int maxFd;
fd_set inset;
fd_set watchset; 
struct timeval tv;
struct timeval *timeoutPtr;
int fd;
int retVal;
int ret;
int result;
char me[30];
pid_t childPid;
int yBytes = 0;
int kaCounter = 0;
SIMPL_REC receiverInfo;

// get pid
childPid = getpid();

// set the name of this forked program
sprintf(me, "%s_%d", SURROGATE_TCP_S_CHILD, childPid);

// ensure socket ownership
if (fcntl(surSock, F_SETOWN, childPid) == -1)
	{  
	_simpl_log("%s: socket 1 ownership error-%s\n", fn, strerror(errno));
	nameLocateReply(-1);
	exit(-1);
	}

#ifdef BUFFERED
	// ensure socket ownership
	if (fcntl(dupSock, F_SETOWN, childPid) == -1)
		{  
		_simpl_log("%s: socket 2 ownership error-%s\n", fn, strerror(errno));
		nameLocateReply(-1);
		exit(-1);
		}
#endif

// attach a name for this surrogate
if (name_attach(me, NULL) == -1)
	{  
	_simpl_log("%s: name attach error-%s\n", fn, whatsMyError());
	nameLocateReply(-1);
	exit(-1);
	}
	
// perform the local name locate for the asked for process
fd = locateLocalName(&receiverInfo);

// set the result off the local name locate
result = (fd == -1) ? -1 : 0;

// reply the results of the local name_locate()
if (nameLocateReply(result) == -1)
	{
	_simpl_log("%s: name locate reply error\n", fn);
	exit(-1);
	}

// if the results of the local name_locate() are -1, we are not needed
if (result == -1)
	{
	exit(0);
	}

// arm select for the TCP connection and the timeout
fds[0] = surSock;
fds[1] = whatsMyReplyfd();
maxFd = (fds[0] > fds[1]) ? fds[0] : fds[1];
maxFd += 1;

FD_ZERO(&watchset);
FD_SET(fds[0], &watchset);
FD_SET(fds[1], &watchset);

// initialize timeout structure
if (!kaTimeout)
	{
	timeoutPtr = NULL;
	}
else
	{
	tv.tv_sec = kaTimeout;
	tv.tv_usec = 0;
	timeoutPtr = &tv;
	}
 
// handle incoming messages destined for a local receiver
while (1)
	{
	// reset the fd set each time
	inset = watchset;

	// let select be the trigger on the file descriptors/timer
	retVal = select(maxFd, &inset, NULL, NULL, timeoutPtr);
	if (retVal > 0) 
		{
		if (FD_ISSET(fds[0], &inset))
			{
			// a message from surrogate partner
			ret = hndlMessage(fd, &kaCounter, &yBytes);
			if (ret == -1)
				{
				errorReply();
				exit(-1);
				}
			}
		else if (FD_ISSET(fds[1], &inset))
			{
			// a reply from local receiver
			ret = hndlReply(yBytes);
			if (ret == -1)
				{
				errorReply();
				exit(-1);
				}
			}
		else 
			{
			// unknown fd error
			_simpl_log("%s: fd error on select\n", fn);
			}
		}
	else if (retVal == 0) 
		{
		// a good opportunity to check on the local receiver
		if (simplCheckProcess(&receiverInfo) == -1)
			{
			// could also send a close message to surrogate_r partner
			// surrogate_r should pick up on the ka failures
			exit(0);
			}
				
		// select timer has gone off check the kaCounter
		kaCounter += 1;
		if (kaCounter > KEEP_ALIVE_FAIL_LIMIT)
			{
			// we assume that our surrogate partner is no longer
			exit(0);
			}
	
		// timer values must be reset each time as necessary
		tv.tv_sec = kaTimeout;
		tv.tv_usec = 0;
		timeoutPtr = &tv;
		}
	else
		{
		// select error
		_simpl_log("%s: select error-%s\n", fn, strerror(errno));
		}
	}
}

/**********************************************************************
FUNCTION:	locateLocalName(SIMPL_REC *)

PURPOSE:	This function reports the results of a local name_locate()
			to the calling function.

RETURNS:	int
**********************************************************************/	

int locateLocalName(SIMPL_REC *receiverInfo)
{
// memArea is global
// amemArea is global
const char *fn = "locateLocalName";

#ifdef SUR_CHR	/********** char message **********/
	const int size = sizeof(SUR_NAME_LOCATE_CHR_MSG) - sizeof(SUR_MSG_CHR_HDR);
	SUR_NAME_LOCATE_CHR_MSG *surMsg;
#else			/********** binary message **********/
	const int size = sizeof(SUR_NAME_LOCATE_MSG) - sizeof(SUR_MSG_HDR);
	SUR_NAME_LOCATE_MSG *surMsg;
#endif

// read the balance of the SUR_NAME_LOCATE_CHR_MSG/SUR_NAME_LOCATE_MSG
if (surRead(amemArea, size) == -1)
	{
	_simpl_log("%s: read error-%s\n", fn, strerror(errno));
	return(-1);
	}
	
// interpret message
#ifdef SUR_CHR	/********** char message **********/
	surMsg = (SUR_NAME_LOCATE_CHR_MSG *)memArea;
#else			/********** binary message **********/
	surMsg = (SUR_NAME_LOCATE_MSG *)memArea;
#endif

// set process record for possible later use
if (simplSetReceiverParms(surMsg->rProgramName, receiverInfo) == -1)
	{		
	_simpl_log("%s: set receiver parms failed\n", fn);
	return(-1);
	}
	
// does this process exist locally?
return( name_locate(surMsg->rProgramName) );
}

/**********************************************************************
FUNCTION:	hndlMessage(int, int *, int *)

PURPOSE:	Deal with an incoming messages from the receiver surrogate.

RETURNS:	int	
**********************************************************************/	

int hndlMessage(int fd, int *kaCounter, int *yBytes)
{
// memArea is global
// amemSize is global
const static char *fn = "hndlMessage";
int nBytes;
int maxBytes;
int token;
int proxyValue;

#ifdef SUR_CHR	/********** char message **********/
	const static int hdrSize = sizeof(SUR_MSG_CHR_HDR);
	SUR_MSG_CHR_HDR *hdr;
	const static int outSize = sizeof(SUR_KA_REPLY_CHR_MSG);
	SUR_KA_REPLY_CHR_MSG *out;
	SUR_CHR_PROXY_MSG *prox;
#else			/********** binary message **********/
	const static int hdrSize = sizeof(SUR_MSG_HDR);
	SUR_MSG_HDR *hdr;
	const static int outSize = sizeof(SUR_KA_REPLY_MSG);
	SUR_KA_REPLY_MSG *out;
	SUR_PROXY_MSG *prox;
#endif

// read the header portion of the message
if (surRead(memArea, hdrSize) == -1)
	{
	_simpl_log("%s: read error on header-%s\n", fn, strerror(errno));
	return(-1);
	}

// interpret the message header
#ifdef SUR_CHR	/********** char message **********/
	hdr = (SUR_MSG_CHR_HDR *)memArea;
	token = stobSI(hdr->token, intWidth);
#else			/********** binary message **********/
	hdr = (SUR_MSG_HDR *)memArea;
	token = hdr->token;
#endif

// react on token value
switch (token)
	{
	case SUR_SEND:
		#ifdef SUR_CHR	/********** char message **********/
			// set send and reply message sizes
			nBytes = stobUI(hdr->nbytes, intWidth);
			*yBytes = stobUI(hdr->ybytes, intWidth);
		#else			/********** binary message **********/
			// set send and reply message sizes
			nBytes = hdr->nbytes;
			*yBytes = hdr->ybytes;
		#endif
		
		// what is the largest message; sent or replied?
		maxBytes = (nBytes > *yBytes) ? nBytes : *yBytes;

		// check for adequate buffer size
		if (maxBytes > amemSize)
			{
			if (adjustMemory(maxBytes) == -1)
				{ 
				_simpl_log("%s: memory allocation error\n", fn);
				return(-1);
				}
			}
			
		// read in the data portion of the message
		if (surRead(amemArea, nBytes) == -1)
			{
			_simpl_log("%s: read error on message-%s\n", fn, strerror(errno));
			return(-1);
			}	

		// send message to local receiver process
		if (_simpl_postMsg(fd, amemArea, nBytes, *yBytes) == -1)
			{
			_simpl_log("%s: send error-%s\n", fn, whatsMyError());
			return(-1);
			}
		break;

	case SUR_PROXY:
		#ifdef SUR_CHR	/********** char message **********/
			// set send and reply message sizes
			nBytes = stobUI(hdr->nbytes, intWidth);
		#else			/********** binary message **********/
			// set send and reply message sizes
			nBytes = hdr->nbytes;
		#endif
		
		// read in the data portion of the message
		if (surRead(amemArea, nBytes) == -1)
			{
			_simpl_log("%s: read error on proxy-%s\n", fn, strerror(errno));
			return(-1);
			}	

		// extract the proxy value
		#ifdef SUR_CHR	/********** char message **********/
			prox = (SUR_CHR_PROXY_MSG *)memArea;
			proxyValue = stobSI(prox->proxyValue, intWidth);
		#else			/********** binary message **********/
			prox = (SUR_PROXY_MSG *)memArea;
			proxyValue = prox->proxyValue;
		#endif

		// trigger proxy
		if (Trigger(fd, proxyValue) == -1)
			{
			_simpl_log("%s: trigger error-%s\n", fn, whatsMyError());
			return(-1);
			}
		break;

	case SUR_ALIVE:
		*kaCounter = 0;

		// compose the reply message
		#ifdef SUR_CHR	/********** char message **********/
			out = (SUR_KA_REPLY_CHR_MSG *)memArea;
			btosSI(SUR_ALIVE_REPLY, out->hdr.token, intWidth);
		#else			/********** binary message **********/
			out = (SUR_KA_REPLY_MSG *)memArea;
			out->hdr.token = SUR_ALIVE_REPLY;
			out->hdr.nbytes = 0;
		#endif

		// send reply back to surrogate partner
		if (surWrite(out, outSize) == -1)
			{
			_simpl_log("%s: write error-%s\n", fn, strerror(errno));
			return(-1);
			}
		break;
		
	case SUR_CLOSE:
		exit(0);

	default:
		_simpl_log("%s: unknown message token=%d\n", fn, token);
		return(-1);
	}

return(0);
}

/**********************************************************************
FUNCTION:	hndlReply(int)

PURPOSE:	Deal with replies from the receiver process.

RETURNS:	int	
**********************************************************************/	

int hndlReply(int yBytes)
{
// memArea is global
// amemArea is global
const static char *fn = "hndlReply";
int replySize;

#ifdef SUR_CHR	/********** char message **********/
	const static int hdrSize = sizeof(SUR_MSG_CHR_HDR);
	SUR_MSG_CHR_HDR *hdr = (SUR_MSG_CHR_HDR *)memArea;
#else			/********** binary message **********/
	const static int hdrSize = sizeof(SUR_MSG_HDR);
	SUR_MSG_HDR *hdr = (SUR_MSG_HDR *)memArea;
#endif

// get the reply message
replySize = _simpl_readReply(amemArea, yBytes);
if (replySize == -1)
	{
	_simpl_log("%s: send error-%s\n", fn, whatsMyError());
	return(-1);
	}

// build the reply header
#ifdef SUR_CHR	/********** char message **********/
	btosSI(SUR_REPLY, hdr->token, intWidth);
	btosSI(replySize, hdr->nbytes, intWidth);
#else			/********** binary message **********/
	hdr->token = SUR_REPLY;
	hdr->nbytes = replySize;
#endif

// send reply back to surrogate partner 
if (surWrite(memArea, hdrSize + replySize) == -1)
	{
	_simpl_log("%s: write error reply-%s\n", fn, strerror(errno));
	return(-1);
	}

return(0);
}
	
/**********************************************************************
FUNCTION:	errorReply(void)

PURPOSE:	Send an error reply back to the receiver surrogate
			indicating problems at this end.

RETURNS:	void	
**********************************************************************/	

void errorReply()
{
// memArea is global
const static char *fn = "errorReply";

#ifdef SUR_CHR	/********** char message **********/
	const static int size = sizeof(SUR_MSG_CHR_HDR);
	SUR_MSG_CHR_HDR *out;
#else			/********** binary message **********/
	const static int size = sizeof(SUR_MSG_HDR);
	SUR_MSG_HDR *out;
#endif

// concoct the error reply
#ifdef SUR_CHR	/********** char message **********/
	out = (SUR_MSG_CHR_HDR *)memArea;
	btosSI(SUR_ERROR, out->token, intWidth);
#else			/********** binary message **********/
	out = (SUR_MSG_HDR *)memArea;
	out->token = SUR_ERROR;
	out->nbytes = 0;
#endif

// send error reply 
if (surWrite(memArea, size) == -1)
	{
	_simpl_log("%s: write error-%s\n", fn, strerror(errno));
	}
}
