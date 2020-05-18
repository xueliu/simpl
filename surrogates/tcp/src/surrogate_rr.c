/***********************************************************************

FILE:			surrogate_r.c

DATE:			05 Oct 28

DESCRIPTION:	This program is run as a fork from surrogate_R.c

NOTES:			receives the following messages:
				1. SUR_NAME_LOCATE (from local sender)
				2. SUR_SEND (from local sender)
				3. SUR_PROXY (from local sender)
				4. SUR_CLOSE (from local sender)
				5. SUR_ALIVE_REPLY (from remote surrogate_s)

				sends the following messages:
				1. SUR_NAME_LOCATE (to remote surrogate_s)
				2. SUR_SEND (to remote surrogate_s)
				3. SUR_PROXY (to remote surrogate_s)
				4. SUR_CLOSE (to remote surrogate_s)
				5. SUR_ALIVE (to remote surrogate_s)
				6. reply messages back to the local sender.

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
$Log: surrogate_rr.c,v $
Revision 1.1  2011/03/23 17:52:34  bobfcsoft
new file replaces surrogate_r.c

Revision 1.1  2007/01/09 15:26:00  bobfcsoft
restore from tcp_x

Revision 1.5  2006/04/24 22:17:33  bobfcsoft
added b_port

Revision 1.4  2006/01/10 15:32:22  bobfcsoft
v3.0 changes

***********************************************************************/

// application header
#define _ALLOC extern
#include "surrogate.h"
#undef _ALLOC

/**********************************************************************
FUNCTION:	surrogate_r(void)

PURPOSE:	Talk via tcp to the surrogate_S	process on another host
			to perform a remote name_locate() of a receiver process on that
			host. If such a process exists, then become a surrogate
			receiver and tcp sender to a remote surrogate tcp receiver
			and sender on that other host thus acting as one side of a
			conduit between a local sender and a remote receiver.

RETURNS:	nothing, it is a forked process and never returns.
**********************************************************************/	

void surrogate_r()
{
// kaTimeout is global
// surSock is global
const char *fn = "surrogate_r";
char me[30];
pid_t childPid;
int fds[3];
int maxFd;
fd_set watchset; 
fd_set inset;
struct timeval tv;
struct timeval *timeoutPtr;
int ret;
int kaCounter = 0;
SIMPL_REC senderInfo;

// detach the child from the parent's simpl shmem
child_detach();

// get the pid
childPid = getpid();

// set the name of this forked program
sprintf(me, "%s_%d", SURROGATE_TCP_R_CHILD, childPid);

// attach a name to this process
if (name_attach(me, NULL) == -1)
	{
	_simpl_log("%s: name attach error-%s\n", fn, whatsMyError());
	exit(-1);
	}

// a remote name locate message will be the first thing to happen
if (hndlRemoteNameLocate(childPid, &senderInfo) == -1)
	{
	_simpl_log("%s: name locate error\n", fn);
	exit(-1);
	}

// for local simpl messages 
fds[0] = whatsMyRecvfd();

// for remote surrogate messages; surSock set in hndlRemoteNameLocate() above 
fds[1] = surSock;

// set select parameters
FD_ZERO(&watchset);
FD_SET(fds[0], &watchset);
FD_SET(fds[1], &watchset);
maxFd = (fds[0] > fds[1]) ? fds[0] : fds[1];
maxFd += 1;

// set timer values in case of keep alive
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

while (1)
	{
	// reset the fd set each time
	inset = watchset;

	// let select be the trigger on the file descriptors/timer
	ret = select(maxFd, &inset, NULL, NULL, timeoutPtr);
	if (ret > 0) 
		{
		if (FD_ISSET(fds[0], &inset))
			{
			// this is a local simpl Send message
			if (hndlSendMsg() == -1)
				{
				_simpl_log("%s: hndlSendMsg error\n", fn);
				exit(-1);
				}
			}
		else if (FD_ISSET(fds[1], &inset))
			{
			// send reply or keep alive reply from the remote surrogate 
			if (hndlRemoteMsg(&kaCounter) == -1)
				{
				_simpl_log("%s: hndlInMsg error\n", fn);
				exit(-1);
				}
			}
		else 
			{
			// unknown fd error
			_simpl_log("%s: unknown fd on select-%s\n", fn, strerror(errno));
			}
		}
	else if (ret == 0) 
		{
		// a good opportunity to check on the local sender
		if (simplCheckProcess(&senderInfo) == -1)
			{
			// sender has gone and somehow left this surrogate behind
			// one posibility is that the sender was SIGKILL'd
			// send a close message to the remote surrogate partner
			hndlClose();
			exit(0);
			}	
		
		// select timer kick returns 0 --> send a keep alive message to surrogate partner
		if (sendKeepAliveMsg(&kaCounter) == -1)
			{
			_simpl_log("%s: sendKeepAliveMsg error\n", fn);
			exit(-1);
			}

		// check the number of keep alive messages sent
		if (kaCounter > KEEP_ALIVE_FAIL_LIMIT)
			{
			_simpl_log("%s: too many keep alive failures\n", fn);
			exit(-1);
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
FUNCTION:	hndlRemoteNameLocate(pid_t, SENDER_REC *)

PURPOSE:	Handle a simpl SUR_NAME_LOCATE call.
			Should only be called once for each surrogate child.

RETURNS:	int	
**********************************************************************/	

int hndlRemoteNameLocate(pid_t childPid, SIMPL_REC *senderInfo)
{
// surSock is global
// sender is global
// memArea is global
// a_port is global
// rFp is global
// wFp is global
const static char *fn = "hndlRemoteNameLocate";
int fds[1];
fd_set watchset; 
int nBytes;
struct timeval tv;
struct timeval *timeoutPtr;
int ret;
SUR_NAME_LOCATE_MSG lin;
SUR_NAME_LOCATE_REPLY lout;

#ifdef SUR_CHR	/********** char message **********/
	const static int inSize = sizeof(SUR_NAME_LOCATE_CHR_REPLY);
	SUR_NAME_LOCATE_CHR_REPLY *rin;
	const static int outSize = sizeof(SUR_NAME_LOCATE_CHR_MSG);
	SUR_NAME_LOCATE_CHR_MSG *rout;
#else			/********** binary message **********/
	const static int inSize = sizeof(SUR_NAME_LOCATE_REPLY);
	SUR_NAME_LOCATE_REPLY *rin;
	const static int outSize = sizeof(SUR_NAME_LOCATE_MSG);
	SUR_NAME_LOCATE_MSG *rout;
#endif

// set to largest message encountered so far
if (adjustMemory(sizeof(SUR_NAME_LOCATE_MSG)) == -1)
	{
	_simpl_log("%s: memory allocation error\n", fn);
	return(-1);
	}

// receive name locate message from the local sender
nBytes = Receive(&sender, NULL, 0);
if (nBytes == -1)
	{
	_simpl_log("%s: receive error-%s\n", fn, whatsMyError());
	return(-1);
	}

// make more certain that this is a name_locate message
if (nBytes != sizeof(SUR_NAME_LOCATE_MSG))
	{
	_simpl_log("%s: incorrect message size\n", fn);
	ReplyError(sender);	
	return(-1);
	} 

// get the message from sender's shmem
simplRcopy(sender, &lin, nBytes);

// check the token
if (lin.hdr.token != SUR_NAME_LOCATE)
	{
	_simpl_log("%s: incorrect message type\n", fn);
	ReplyError(sender);	
	return(-1);
	}

// get sender information
simplSetSenderParms(sender, senderInfo);

// make socket for sending remote name locate and local sender messages
//surSock = initSsock(a_port, lin.rHostName);
surSock = initSsock(b_port, lin.rHostName);
if (surSock == -1)
	{
	_simpl_log("%s: message socket creation error\n", fn);
	replyFailure(sender);
	return(-1);
	}

// ensure socket ownership
if (fcntl(surSock, F_SETOWN, childPid) == -1)
	{
	_simpl_log("%s: message socket ownership error-%s\n", fn, strerror(errno));
	replyFailure(sender);
	return(-1);
	}

#ifdef BUFFERED
	// open buffered read on socket
	rFp = fdopen(surSock, "r");
	if (rFp == NULL)
		{
		_simpl_log("%s: rFp error-%s\n", fn, strerror(errno));
		replyFailure(sender);
		return(-1);
		}

	// open buffered write on socket
	wFp = fdopen(dup(surSock), "w");
	if (wFp == NULL)
		{
		_simpl_log("%s: wFp error-%s\n", fn, strerror(errno));
		replyFailure(sender);
		return(-1);
		}

	/*
	At his point you may want to augment the file stream functionality with 
	calls to setbuf(), setbuffer(), setlinebuf() or setvbuf() with modes such
	as _IOFBF, _IOLBF or _IONBF. For _IONBF (no buffering) you will not need
	a call to fflush() after fwrite() for example. fflush() is merely a call
	to the more primitive write() system call.
	*/
#endif
	
#ifdef SUR_CHR	/********** char message **********/
	rout = (SUR_NAME_LOCATE_CHR_MSG *)memArea;
	btosSI(SUR_NAME_LOCATE, rout->hdr.token, intWidth);
	// rout->hdr.nbytes not required by surrogate_s
	// rout->hdr.ybytes not required by surrogate_s
	memcpy(rout->rProgramName, lin.rProgramName, MAX_PROGRAM_NAME_LEN + 1);
	memcpy(rout->sHostName, lin.sHostName, MAX_HOST_NAME_LEN + 1);
#else			/********** binary message **********/
	rout = &lin;
#endif

// send the name locate message to the other box
if (surWrite(rout, outSize) == -1)
	{
	_simpl_log("%s: write error on name locate msg-%s\n", fn, strerror(errno));
	replyFailure(sender);
	return(-1);
	}

// block and wait for response 
fds[0] = surSock;
FD_ZERO(&watchset);
FD_SET(fds[0], &watchset);

// set time out on wait for response
tv.tv_sec = namelocTimeout;
tv.tv_usec = 0;
timeoutPtr = &tv;

ret = select(fds[0] + 1, &watchset, NULL, NULL, timeoutPtr);
if (ret > 0) 
	{
	if (FD_ISSET(fds[0], &watchset))
		{
		// read the reply to the SUR_NAME_LOCATE (-1 fail !=-1 success)
		if (surRead(memArea, inSize) == -1)
			{
			_simpl_log("%s: read error on name locate reply-%s\n", fn, strerror(errno));
			replyFailure(sender);
			return(-1);
			}

		// interpret replied message
		#ifdef SUR_CHR	/********** char message **********/
			rin = (SUR_NAME_LOCATE_CHR_REPLY *)memArea;
			// lout.hdr.token not read by sender but could be used as a check 
			lout.hdr.token = stobSI(rin->hdr.token, intWidth);
			// lout.hdr.nbytes is not set by surrogate_s
			// lout.hdr.ybytes is not set by surrogate_s
			lout.result = stobSI(rin->result, intWidth); // the important piece of info
		#else			/********** binary message **********/
			rin = (SUR_NAME_LOCATE_REPLY *)memArea;
			// lout.hdr.token not read by sender but could be used as a check 
			lout.hdr.token = rin->hdr.token;
			// lout.hdr.nbytes = sizeof(SUR_NAME_LOCATE_REPLY);
			// lout.hdr.ybytes is not set by surrogate_s
			lout.result = rin->result; // the important piece of info
		#endif

		// reply the search results to the ipc sender
		if (Reply(sender, &lout, sizeof(SUR_NAME_LOCATE_REPLY)) == -1)
			{
			_simpl_log("%s: reply error-%s\n", fn, whatsMyError());
			return(-1);
			}
		}
	else 
		{
		// unknown fd error
		_simpl_log("%s: unknown fd on select-%s\n", fn, strerror(errno));
		}
	}
else if (ret == 0) 
	{
	// select timer kick returns 0 --> have not received a name locate reply
	// no error log; network/hostprocess simply not there
	replyFailure(sender);
	return(-1);
	}
else
	{
	// select error
	_simpl_log("%s: select error-%s\n", fn, strerror(errno));
	replyFailure(sender);
	return(-1);
	}

return(0);
}

/**********************************************************************
FUNCTION:	hndlSendMsg(void)

PURPOSE:	Handle simpl SUR_SEND call from a local sender intended
			for a remote receiver.

RETURNS:	int	

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

int hndlSendMsg()
{
// memArea is global
// amemArea is global
// amemSize is global
// sender is global
const static char *fn = "hndlSendMsg";
int nBytes;
int yBytes;
int maxBytes;
int proxyValue;

#ifdef SUR_CHR	/********** char message **********/
	const static int hdrSize = sizeof(SUR_MSG_CHR_HDR);
	SUR_MSG_CHR_HDR *out;
#else			/********** binary message **********/
	const static int hdrSize = sizeof(SUR_MSG_HDR);
	SUR_MSG_HDR *out;
#endif 

// receive message from the local sender
nBytes = Receive(&sender, NULL, 0);
if (nBytes == -1)
	{
	_simpl_log("%s: receive error-%s\n",fn, whatsMyError());
	return(-1);
	}

// is it a trigger proxy?
if (nBytes < -1)
	{
	// get the value of the proxy
	proxyValue = returnProxy(nBytes);
	
	// is it a close up shop proxy?
	if (proxyValue == PROXY_SHUTDOWN)
		{
		// send a close message to the remote surrogate partner
		hndlClose();
		exit(0);
		}
	else
		{
		return( hndlProxy(proxyValue) );
		}
	}

// what is the size of the expected reply message?
yBytes = simplReplySize(sender);

// which is larger, the sent or the replied message?
maxBytes = (nBytes > yBytes) ? nBytes : yBytes;

// check for adequate buffer size
if (maxBytes > amemSize)
	{
	if (adjustMemory(maxBytes) == -1)
		{ 
		_simpl_log("%s: memory allocation error\n", fn);
		ReplyError(sender);
		return(-1);
		}
	}

// get the message from sender's shmem
simplRcopy(sender, amemArea, nBytes);

// build the message going to the other box	
#ifdef SUR_CHR	/********** char message **********/
	out = (SUR_MSG_CHR_HDR *)memArea;
	btosSI(SUR_SEND, out->token, intWidth);
	btosUI(nBytes, out->nbytes, intWidth);
	btosUI(yBytes, out->ybytes, intWidth);
#else			/********** binary message **********/
	out = (SUR_MSG_HDR *)memArea;
	out->token = SUR_SEND;
	out->nbytes = nBytes;
	out->ybytes = yBytes;
#endif

// send the message via the socket to the other box
if (surWrite(memArea, hdrSize + nBytes) == -1)
	{
	_simpl_log("%s: write error on msg-%s", fn, strerror(errno));
	ReplyError(sender);	
	return(-1);
	}

return(0);
}

/**********************************************************************
FUNCTION:	hndlRemoteMsg(int *)

PURPOSE:	Handle incoming tcp messages from the remote surrogate. 

RETURNS:	int	
**********************************************************************/	

int hndlRemoteMsg(int *kaCounter)
{
// memArea is global
// amemArea is global
// amemSize is global
// sender is global
const static char *fn = "hndlInMsg";
int token;
unsigned numBytes;

#ifdef SUR_CHR	/********** char message **********/
	const static int hdrSize = sizeof(SUR_MSG_CHR_HDR);
	SUR_MSG_CHR_HDR *in;
#else			/********** binary message **********/
	const static int hdrSize = sizeof(SUR_MSG_HDR);
	SUR_MSG_HDR *in;
#endif 

// socket reply message header from other box
if (surRead(memArea, hdrSize) == -1)
	{
	_simpl_log("%s: read error on msg header-%s\n", fn, strerror(errno));
	ReplyError(sender);
	return(-1);
	}			

// interpret the message header
#ifdef SUR_CHR	/********** char message **********/
	in = (SUR_MSG_CHR_HDR *)memArea;
	token = stobSI(in->token, intWidth);
	numBytes = stobUI(in->nbytes, intWidth);
#else			/********** binary message **********/
	in = (SUR_MSG_HDR *)memArea;
	token = in->token;
	numBytes = in->nbytes;
#endif

// reaction based on the message token
switch (token)
	{
	case SUR_REPLY:
		// socket reply message data from other box
		if (surRead(amemArea, numBytes) == -1)
			{
			_simpl_log("%s: read error on msg data-%s\n", fn, strerror(errno));
			ReplyError(sender);	
			return(-1);
			}

		// reply message to local ipc sender
		if (Reply(sender, amemArea, numBytes) == -1)
			{
			_simpl_log("%s: reply error-%s\n", fn, whatsMyError());
			return(-1);
			}
		break;		  

	case SUR_ALIVE_REPLY:
		*kaCounter = 0;
		break;

	case SUR_ERROR:
		ReplyError(sender);	
		// surrogate sender always dies upon this error ==> we are not needed
		exit(-1);

	default:
		_simpl_log("%s: unknown message token\n", fn);
		return(-1);
	}		  

return(0);
}

/**********************************************************************
FUNCTION:	sendKeepAliveMsg(int *)

PURPOSE:	Send a keep alive message to the other surrogate partner.

RETURNS:	int	
**********************************************************************/	

int sendKeepAliveMsg(int *kaCounter)
{
// memArea is global
const static char *fn = "sendKeepAliveMsg";

#ifdef SUR_CHR	/********** char message **********/
	SUR_KA_CHR_MSG *out;
	const static int outSize = sizeof(SUR_KA_CHR_MSG);
#else			/********** binary message **********/
	SUR_KA_MSG *out;
	const static int outSize = sizeof(SUR_KA_MSG);
#endif

// compose the keep alive message
#ifdef SUR_CHR	/********** char message **********/
	out = (SUR_KA_CHR_MSG *)memArea;
	btosSI(SUR_ALIVE, out->hdr.token, intWidth);
#else			/********** binary message **********/
	out = (SUR_KA_MSG *)memArea;
	out->hdr.token = SUR_ALIVE;
	out->hdr.nbytes = 0;  // header only
#endif

// send the keep alive message to the other surrogate's parent 
if (surWrite(out, outSize) == -1)
	{
	_simpl_log("%s: write error-%s\n", fn, strerror(errno));
	return(-1);
	}

// increment the number of keep alive messages sent
*kaCounter += 1;

return(0);
}

/**********************************************************************
FUNCTION:	hndlClose(void)

PURPOSE:	Handle simpl SUR_CLOSE call. This will cause this
			"receiving surrogate" to exit and send a close instruction
			to its remote "send surrogate" partner.

RETURNS:	void	
**********************************************************************/	

void hndlClose()
{
// memArea is global
const char *fn = "hndlClose";

#ifdef SUR_CHR	/********** char message **********/
	const int size = sizeof(SUR_MSG_CHR_HDR);
	SUR_MSG_CHR_HDR *out;
#else			/********** binary message **********/
	const int size = sizeof(SUR_MSG_HDR);
	SUR_MSG_HDR *out;
#endif

// set up close message
#ifdef SUR_CHR	/********** char message **********/
	out = (SUR_MSG_CHR_HDR *)memArea;
	btosSI(SUR_CLOSE, out->token, intWidth);
#else			/********** binary message **********/
	out = (SUR_MSG_HDR *)memArea;
	out->token = SUR_CLOSE;
	out->nbytes = 0;
#endif

// send the message via the socket to the other box
if (surWrite(out, size) == -1)
	{
	_simpl_log("%s: write error on close msg-%s\n", fn, strerror(errno));
	}
}

/**********************************************************************
FUNCTION:	hndlProxy(int)

PURPOSE:	Handle sending a proxy to the remote surrogate_s.

RETURNS:	int	
**********************************************************************/	

int hndlProxy(int proxyValue)
{
// memArea is global
const char *fn = "hndlProxy";

#ifdef SUR_CHR	/********** char message **********/
	const int size = sizeof(SUR_CHR_PROXY_MSG);
	SUR_CHR_PROXY_MSG *out;
#else			/********** binary message **********/
	const int size = sizeof(SUR_PROXY_MSG);
	SUR_PROXY_MSG *out;
#endif

// set up proxy message
#ifdef SUR_CHR	/********** char message **********/
	out = (SUR_CHR_PROXY_MSG *)memArea;
	btosSI(SUR_PROXY, out->hdr.token, intWidth);
	btosSI(sizeof(out->proxyValue), out->hdr.nbytes, intWidth);
	btosSI(proxyValue, out->proxyValue, intWidth);
#else			/********** binary message **********/
	out = (SUR_PROXY_MSG *)memArea;
	out->hdr.token = SUR_PROXY;
	out->hdr.nbytes = sizeof(out->proxyValue);
	out->proxyValue = proxyValue;
#endif

// send the message via the socket to the other box
if (surWrite(out, size) == -1)
	{
	_simpl_log("%s: write error on proxy msg-%s\n", fn, strerror(errno));
	return(-1);
	}

return(0);
}
