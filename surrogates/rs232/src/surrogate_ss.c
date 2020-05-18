/***********************************************************************

FILE:			surrogate_s.c

DATE:			07 July 25

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

    If you discover a bug or add an enhancement contact us on the
    SIMPL project mailing list.
-----------------------------------------------------------------------
REVISIONS:
$Log: surrogate_ss.c,v $
Revision 1.1  2011/03/23 17:50:31  bobfcsoft
new file replaces surrogate_s.c

Revision 1.1  2008/04/21 17:22:57  johnfcsoft
initial


***********************************************************************/

// application header
#define _ALLOC extern
#include "surrogate.h"
#undef _ALLOC

/**********************************************************************
FUNCTION:	surrogate_s(int)

PURPOSE:	Handle a remote name locate request and act as a conduit
			by receiving serial messages from a remote surrogate receiver
			and sending them on to the actual receiver.

RETURNS:	nothing, it is a forked process and never returns.	
**********************************************************************/	

void surrogate_s()
{
// kaTimeout is global
// kaHostName is global
// serialWriter is global
// surSpid is global
int fds[2];
int maxFd;
fd_set inset;
fd_set watchset; 
struct timeval tv;
struct timeval *timeoutPtr;
int retVal;
int ret;
int id;
int result;
char me[30];
int yBytes = 0;
int kaCounter = 0;
SIMPL_REC receiverInfo;

// detach the child from the parent's simpl shmem
child_detach();

// get this program's pid
surSpid = getpid();

// set the name of this forked program
sprintf(me, "%s_%d", SURROGATE_RS232_S_CHILD, surSpid);

// attach a name for this surrogate
if (name_attach(me, NULL) == -1)
	{  
	_simpl_log("%s: name attach error-%s\n", me, whatsMyError());
	nameLocateReply(-1);
	exit(-1);
	}
	
// name locate the serial writer
serialWriter = name_locate(RS232_WRITER);
if (serialWriter == -1)
	{
	_simpl_log("%s: name locate on serial writer error-%s\n", me,
		whatsMyError());
	exit(-1);
	}

// perform the local name locate for the asked for process
id = locateLocalName(&receiverInfo);

// set the result off the local name locate
result = (id == -1) ? -1 : surSpid;

// reply the results of the local name_locate()
if (nameLocateReply(result) == -1)
	{
	_simpl_log("%s: name locate reply error\n", me);
	exit(-1);
	}

// if the results of the local name_locate() are -1, we are not needed
if (result == -1)
	{
	exit(0);
	}

// arm select
fds[0] = whatsMyRecvfd();
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
			ret = hndlMessage(id, &kaCounter, &yBytes);
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
			_simpl_log("%s: fd error on select\n", me);
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
		_simpl_log("%s: select error-%s\n", me, strerror(errno));
		}
	}
}

/**********************************************************************
FUNCTION:	locateLocalName(SIMPL_REC *)

PURPOSE:	This function reports the results of a local name_locate()
			to the calling function.

RETURNS:	int

NOTES:		The name locate information was read into global memory by the
			parent process prior to spawning this process.
**********************************************************************/

int locateLocalName(SIMPL_REC *receiverInfo)
{
// inMsgArea is global
// surRpid is global
const char *fn = "locateLocalName_rs232";

#ifdef SUR_CHR	/********** char message **********/
	SUR_NAME_LOCATE_CHR_MSG *surMsg = (SUR_NAME_LOCATE_CHR_MSG *)inMsgArea;
	// we need the pid of the remote surrogate_r
	surRpid = stobSI(surMsg->hdr.surPid, intWidth);
#else			/********** binary message **********/
	SUR_NAME_LOCATE_MSG *surMsg = (SUR_NAME_LOCATE_MSG *)inMsgArea;
	// we need the pid of the remote surrogate_r
	surRpid = surMsg->hdr.surPid;
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
FUNCTION:	nameLocateReply(int)

PURPOSE:	This function reports the results of a remote name_locate()
			to the calling receiver surrogate.

RETURNS:	void
**********************************************************************/	

int nameLocateReply(int result)
{
// serialWriter is global, set in surrogate_s
// surRpid is global
const char *fn = "nameLocateReply_rs232";

// build the reply message
#ifdef SUR_CHR	/********** char message **********/
	const int size = sizeof(SUR_NAME_LOCATE_CHR_REPLY);
	SUR_NAME_LOCATE_CHR_REPLY reply;
	btosSI(SUR_REPLY, reply.hdr.token, intWidth);
	btosSI(size - sizeof(SUR_MSG_CHR_HDR), reply.hdr.nbytes, intWidth);
	btosSI(surRpid, reply.hdr.surPid, intWidth);
	btosSI(result, reply.result, intWidth);
#else			/********** binary message **********/
	const static int size = sizeof(SUR_NAME_LOCATE_REPLY);
	SUR_NAME_LOCATE_REPLY reply;
	reply.hdr.token = SUR_REPLY;
	reply.hdr.nbytes = size - sizeof(SUR_MSG_HDR);
	reply.hdr.surPid = surRpid;
	reply.result = result;
#endif

// send a message to the rs232 serial writer
if (Send(serialWriter, &reply, NULL, size, 0) == -1)
	{
	_simpl_log("%s: send error-%s\n", fn, whatsMyError());
	return(-1);
	}

return(0);
}

/**********************************************************************
FUNCTION:	hndlMessage(int, int *, int *)

PURPOSE:	Deal with an incoming messages from the receiver surrogate.

RETURNS:	int	
**********************************************************************/	

int hndlMessage(int fd, int *kaCounter, int *yBytes)
{
// inMsgArea is global
// outMsgArea is global
// serialWriter is global
const static char *fn = "hndlMessage_rs232";
int nBytes;
int token;
int proxyValue;
char *serialR;
int msgSize;

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

// receive message from the serial reader
msgSize = Receive(&serialR, NULL, 0);
if (msgSize == -1)
	{
	_simpl_log("%s: receive error-%s\n", fn, whatsMyError());
	return(-1);
	}

// set to largest message encountered so far
if (checkMemory(IN, msgSize) == -1)
	{
	_simpl_log("%s: memory allocation error\n", fn);
	return(-1);
	}

// get the message from sender's shmem
simplRcopy(serialR, inMsgArea, msgSize);

// reply to serial reader
if (Reply(serialR, NULL, 0) == -1)
	{
	_simpl_log("%s: reply to serial reader error\n", fn);
	return(-1);
	}

// interpret the message header
#ifdef SUR_CHR	/********** char message **********/
	hdr = (SUR_MSG_CHR_HDR *)inMsgArea;
	token = stobSI(hdr->token, intWidth);
#else			/********** binary message **********/
	hdr = (SUR_MSG_HDR *)inMsgArea;
	token = hdr->token;
#endif

// react on token value
switch (token)
	{
	case SUR_SEND:
		#ifdef SUR_CHR	/********** char message **********/
			nBytes = stobUI(hdr->nbytes, intWidth);
			*yBytes = stobUI(hdr->ybytes, intWidth);
		#else			/********** binary message **********/
			// set send and reply message sizes
			nBytes = hdr->nbytes;
			*yBytes = hdr->ybytes;
		#endif
		
		// send message to local receiver process
		if (_simpl_postMsg(fd, inMsgArea + hdrSize, nBytes, *yBytes) == -1)
			{
			_simpl_log("%s: send error-%s\n", fn, whatsMyError());
			return(-1);
			}
		break;

	case SUR_PROXY:
		// extract the proxy value
		#ifdef SUR_CHR	/********** char message **********/
			prox = (SUR_CHR_PROXY_MSG *)inMsgArea;
			proxyValue = stobSI(prox->proxyValue, intWidth);
		#else			/********** binary message **********/
			prox = (SUR_PROXY_MSG *)inMsgArea;
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
			out = (SUR_KA_REPLY_CHR_MSG *)outMsgArea;
			btosSI(SUR_ALIVE_REPLY, out->hdr.token, intWidth);
			btosSI(0, out->hdr.nbytes, intWidth);
			btosSI(surRpid, out->hdr.surPid, intWidth);
		#else			/********** binary message **********/
			out = (SUR_KA_REPLY_MSG *)outMsgArea;
			out->hdr.token = SUR_ALIVE_REPLY;
			out->hdr.nbytes = 0;
			out->hdr.surPid = surRpid;
		#endif

		// send reply back to surrogate partner
		if (Send(serialWriter, out, NULL, outSize, 0) == -1)
			{
			_simpl_log("%s: send error to serial writer-%s\n", fn, whatsMyError());
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
// outMsgArea is global
// serialWriter is global
const static char *fn = "hndlReply_rs232";
int replySize;

#ifdef SUR_CHR	/********** char message **********/
	const static int hdrSize = sizeof(SUR_MSG_CHR_HDR);
#else			/********** binary message **********/
	const static int hdrSize = sizeof(SUR_MSG_HDR);
#endif

// is there enough outgoing message memory?
if (checkMemory(OUT, yBytes) == -1)
	{ 
	_simpl_log("%s: memory allocation error\n", fn);
	return(-1);
	}

// get the reply message
replySize = _simpl_readReply(outMsgArea + hdrSize, yBytes);
if (replySize == -1)
	{
	_simpl_log("%s: readReply error-%s\n", fn, whatsMyError());
	return(-1);
	}

// build the reply header
#ifdef SUR_CHR	/********** char message **********/
	SUR_MSG_CHR_HDR *hdr = (SUR_MSG_CHR_HDR *)outMsgArea;
	btosSI(SUR_REPLY, hdr->token, intWidth);
	btosSI(replySize, hdr->nbytes, intWidth);
	btosSI(surRpid, hdr->surPid, intWidth);
#else			/********** binary message **********/
	SUR_MSG_HDR *hdr = (SUR_MSG_HDR *)outMsgArea;
	hdr->token = SUR_REPLY;
	hdr->nbytes = replySize;
	hdr->surPid = surRpid;
#endif

// send reply back to surrogate partner via the rs232 serial writer
if (Send(serialWriter, outMsgArea, NULL, hdrSize + replySize, 0) == -1)
	{
	_simpl_log("%s: send error reply-%s\n", fn, whatsMyError());
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
// serialWriter is global
const static char *fn = "errorReply_rs232";

// concoct the error reply
#ifdef SUR_CHR	/********** char message **********/
	const static int size = sizeof(SUR_MSG_CHR_HDR);
	SUR_MSG_CHR_HDR out;
	btosSI(SUR_ERROR, out.token, intWidth);
	btosSI(0, out.nbytes, intWidth);
	btosSI(surRpid, out.surPid, intWidth);
#else			/********** binary message **********/
	const static int size = sizeof(SUR_MSG_HDR);
	SUR_MSG_HDR out;
	out.token = SUR_ERROR;
	out.nbytes = 0;
	out.surPid = surRpid;
#endif

// send a message to the rs232 serial writer
if (Send(serialWriter, &out, NULL, size, 0) == -1)
	{
	_simpl_log("%s: send error-%s\n", fn, whatsMyError());
	}
}
