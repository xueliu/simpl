/***********************************************************************

FILE:			surrogate_r.c

DATE:			07 July 23

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

    If you discover a bug or add an enhancement contact us on the
    SIMPL project mailing list.
-----------------------------------------------------------------------
REVISIONS:
$Log: surrogate_rr.c,v $
Revision 1.1  2011/03/23 17:49:41  bobfcsoft
new file replaces surrogate_r.c

Revision 1.1  2008/04/21 17:21:26  johnfcsoft
initial


***********************************************************************/

// application header
#define _ALLOC extern
#include "surrogate.h"
#undef _ALLOC

/**********************************************************************
FUNCTION:	surrogate_r(void)

PURPOSE:	Talk via serial to the rs232 reader process on another host
			to perform a remote name_locate() of a receiver process on that
			host. If such a process exists, then become a surrogate
			receiver and serialsender to a remote surrogate serial receiver
			and sender on that other host thus acting as one side of a
			conduit between a local sender and a remote receiver.

RETURNS:	nothing, it is a forked process and never returns.
**********************************************************************/	

void surrogate_r()
{
// serialWriter is global
// kaTimeout is global
// surRpid is global
char me[30];
int fds[1];
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
surRpid = getpid();

// set the name of this forked program
sprintf(me, "%s_%d", SURROGATE_RS232_R_CHILD, surRpid);

// attach a name to this process
if (name_attach(me, NULL) == -1)
	{
	_simpl_log("%s: name attach error-%s\n", me, whatsMyError());
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

// a remote name locate message will be the first thing to happen
if (hndlRemoteNameLocate(&senderInfo) == -1)
	{
	_simpl_log("%s: name locate error\n", me);
	exit(-1);
	}

// for local simpl messages 
fds[0] = whatsMyRecvfd();

// set select parameters
FD_ZERO(&watchset);
FD_SET(fds[0], &watchset);

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

	// let select be the trigger on the file descriptor/timer
	ret = select(fds[0]+1, &inset, NULL, NULL, timeoutPtr);
	if (ret > 0) 
		{
		if (FD_ISSET(fds[0], &inset))
			{
			// this is a local simpl Send message
			// or send reply/keep alive reply from remote surrogate
			if (hndlMsg(&kaCounter) == -1)
				{
				_simpl_log("%s: hndlSendMsg error\n", me);
				exit(-1);
				}
			}
		else 
			{
			// unknown fd error
			_simpl_log("%s: unknown fd on select-%s\n", me,
				strerror(errno));
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
			_simpl_log("%s: sendKeepAliveMsg error\n", me);
			exit(-1);
			}

		// check the number of keep alive messages sent
		if (kaCounter > KEEP_ALIVE_FAIL_LIMIT)
			{
			_simpl_log("%s: too many keep alive failures\n", me);
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
		_simpl_log("%s: select error-%s\n", me, strerror(errno));
		}
	}
}
		
/**********************************************************************
FUNCTION:	hndlRemoteNameLocate(pid_t, SENDER_REC *)

PURPOSE:	Handle a simpl SUR_NAME_LOCATE call.
			Should only be called once for each surrogate child.

RETURNS:	int	
**********************************************************************/	

int hndlRemoteNameLocate(SIMPL_REC *senderInfo)
{
// surSpid is global
// surRpid is global
const static char *fn = "hndlRemoteNameLocate_rs232";
int fds[1];
fd_set watchset; 
int nBytes;
struct timeval tv;
struct timeval *timeoutPtr;
int ret;
SUR_NAME_LOCATE_MSG lin;
SUR_NAME_LOCATE_REPLY lout;
char *serialR;
char *sender;

#ifdef SUR_CHR	/********** char message **********/
	const static int inSize = sizeof(SUR_NAME_LOCATE_CHR_REPLY);
	SUR_NAME_LOCATE_CHR_REPLY rin;
	const static int outSize = sizeof(SUR_NAME_LOCATE_CHR_MSG);
	SUR_NAME_LOCATE_CHR_MSG rout;
#else			/********** binary message **********/
	const static int inSize = sizeof(SUR_NAME_LOCATE_REPLY);
	SUR_NAME_LOCATE_REPLY rin;
	const static int outSize = sizeof(SUR_NAME_LOCATE_MSG);
	SUR_NAME_LOCATE_MSG rout;
#endif

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

/*
the remote host surrogate_s needs to know this pid so that when it sends 
messages to this program the local serial_r knows how to route it here. The
pid is used in the SIMPL name
*/
lin.hdr.surPid = surRpid;

// get sender information
simplSetSenderParms(sender, senderInfo);
	
#ifdef SUR_CHR	/********** char message **********/
	btosSI(SUR_NAME_LOCATE, rout.hdr.token, intWidth);
	btosSI(lin.hdr.nbytes, rout.hdr.nbytes, intWidth);
	btosSI(lin.hdr.ybytes, rout.hdr.ybytes, intWidth);
	btosSI(lin.hdr.surPid, rout.hdr.surPid, intWidth);
	memcpy(rout.rProgramName, lin.rProgramName, MAX_PROGRAM_NAME_LEN + 1);
	memcpy(rout.rHostName, lin.rHostName, MAX_HOST_NAME_LEN + 1);
	memcpy(rout.sHostName, lin.sHostName, MAX_HOST_NAME_LEN + 1);
#else			/********** binary message **********/
	rout = lin;
#endif

// send the name locate message to the other box via the rs232_w program
if (Send(serialWriter, &rout, NULL, outSize, 0) == -1)
	{
	_simpl_log("%s: send error to serial writer on name locate msg-%s\n",
		fn, whatsMyError());
	replyFailure(sender);
	return(-1);
	}

// block and wait for response 
fds[0] = whatsMyRecvfd();
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
		// receive message from the local serial reader program
		if (Receive(&serialR, &rin, inSize) == -1)
			{
			_simpl_log("%s: receive error on name locate reply-%s\n",fn, whatsMyError());
			replyFailure(sender);
			return(-1);
			}

		// an empty reply to the serial reader
		if (Reply(serialR, NULL, 0) == -1)
			{
			_simpl_log("%s: reply error to serial reader-%s\n", fn, whatsMyError());
			return(-1);
			}

		// interpret replied message
		#ifdef SUR_CHR	/********** char message **********/
			// lout.hdr.token not read by sender
			lout.hdr.token = stobSI(rin.hdr.token, intWidth);
			// lout.hdr.nbytes is not set by surrogate_s
			// lout.hdr.ybytes is not set by surrogate_s
			surSpid = stobSI(rin.result, intWidth);
			lout.result = stobSI(rin.result, intWidth); // important
		#else			/********** binary message **********/
			// lout.hdr.token not read by sender
			lout.hdr.token = rin.hdr.token;
			// lout.hdr.nbytes = sizeof(SUR_NAME_LOCATE_REPLY);
			// lout.hdr.ybytes is not set by surrogate_s
			surSpid = rin.result;
			lout.result = rin.result; // important
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
FUNCTION:	hndlMsg(int *)

PURPOSE:	Handle simpl SUR_SEND call from a local sender intended
			for a remote receiver.

RETURNS:	int	
**********************************************************************/	

int hndlMsg(int *kaCounter)
{
// inMsgArea is global
// outMsgArea is global
// outMsgSize is global
// inMsgSize is global
// surSpid is global
const static char *fn = "hndlMsg_rs232";
int nBytes;
int yBytes;
int maxBytes;
int proxyValue;
SIMPL_REC senderInfo;
int token;
unsigned numBytes;
char *sender;
char *serialSender;
static char *localSender = NULL;

#ifdef SUR_CHR	/********** char message **********/
	const static int hdrSize = sizeof(SUR_MSG_CHR_HDR);
	SUR_MSG_CHR_HDR *in;
	SUR_MSG_CHR_HDR *out;
#else			/********** binary message **********/
	const static int hdrSize = sizeof(SUR_MSG_HDR);
	SUR_MSG_HDR *in;
	SUR_MSG_HDR *out;
#endif 

// receive message from the local sender or the serial reader
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

// get sender information
simplSetSenderParms(sender, &senderInfo);

if ( !strcmp(senderInfo.whom, RS232_READER) ) // from the serial reader
	{
	// set the sender
	serialSender = sender;

	// do we need more memory for the incoming message?
	if (checkMemory(IN, nBytes) == -1)
		{
		_simpl_log("%s: in memory allocation error\n", fn);
		return(-1);
		}

	// get the message from serial reader's shmem
	simplRcopy(serialSender, inMsgArea, nBytes);

	// null reply to the serial reader
	if (Reply(serialSender, NULL, 0) == -1)
		{
		_simpl_log("%s: reply to serial reader error\n", fn);
		return(-1);
		}

	// interpret the message header
	#ifdef SUR_CHR	/********** char message **********/
		in = (SUR_MSG_CHR_HDR *)inMsgArea;
		token = stobSI(in->token, intWidth);
		numBytes = stobUI(in->nbytes, intWidth);
	#else			/********** binary message **********/
		in = (SUR_MSG_HDR *)inMsgArea;
		token = in->token;
		numBytes = in->nbytes;
	#endif

	// reaction based on the message token
	switch (token)
		{
		case SUR_REPLY:
			// reply message from remote receiver to local ipc sender
			if (Reply(localSender, inMsgArea + hdrSize, numBytes) == -1)
				{
				_simpl_log("%s: reply error-%s\n", fn, whatsMyError());
				return(-1);
				}
			break;

		case SUR_ALIVE_REPLY:
			*kaCounter = 0;
			break;

		case SUR_ERROR:
			ReplyError(localSender);
			// surrogate sender always dies upon this error ==> we are not needed
			exit(-1);

		default:
			_simpl_log("%s: unknown message token\n", fn);
			return(-1);
		}	
	}
else // from the local sender
	{
	// set the sender
	localSender = sender;

	// what is the size of the expected reply message?
	yBytes = simplReplySize(localSender);

	// which is larger, the sent or the replied message?
	maxBytes = (nBytes > yBytes) ? nBytes : yBytes;

	// check for adequate buffer size
	if (checkMemory(OUT, maxBytes) == -1)
		{ 
		_simpl_log("%s: memory allocation error\n", fn);
		ReplyError(sender);
		return(-1);
		}

	// get the message from sender's shmem
	simplRcopy(localSender, outMsgArea + hdrSize, nBytes);

	// build the message going to the other box	
	#ifdef SUR_CHR	/********** char message **********/
		out = (SUR_MSG_CHR_HDR *)outMsgArea;
		btosSI(SUR_SEND, out->token, intWidth);
		btosUI(nBytes, out->nbytes, intWidth);
		btosUI(yBytes, out->ybytes, intWidth);
		btosUI(surSpid, out->surPid, intWidth);
	#else			/********** binary message **********/
		out = (SUR_MSG_HDR *)outMsgArea;
		out->token = SUR_SEND;
		out->nbytes = nBytes;
		out->ybytes = yBytes;
		out->surPid = surSpid;
	#endif

	// send the message via serial to the other box
	if (Send(serialWriter, outMsgArea, NULL, hdrSize + nBytes, 0) == -1)
		{
		_simpl_log("%s: send error on msg-%s\n", fn, whatsMyError());
		ReplyError(sender);	
		return(-1);
		}
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
// serialWriter is global
const static char *fn = "sendKeepAliveMsg_rs232";

#ifdef SUR_CHR	/********** char message **********/
	const static int size = sizeof(SUR_KA_CHR_MSG);
	SUR_KA_CHR_MSG out;
	btosSI(SUR_ALIVE, out.hdr.token, intWidth);
	btosSI(0, out.hdr.nbytes, intWidth);
	btosSI(surSpid, out.hdr.surPid, intWidth);
#else			/********** binary message **********/
	const static int size = sizeof(SUR_KA_MSG);
	SUR_KA_MSG out;
	out.hdr.token = SUR_ALIVE;
	out.hdr.nbytes = 0;
	out.hdr.surPid = surSpid;
#endif

// send the keep alive message to the other surrogate's parent 
if (Send(serialWriter, &out, NULL, size, 0) == -1)
	{
	_simpl_log("%s: send error on close msg-%s\n", fn, whatsMyError());
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
// serialWriter is global
// surSpid is global
const char *fn = "hndlClose_rs232";

#ifdef SUR_CHR	/********** char message **********/
	const int size = sizeof(SUR_MSG_CHR_HDR);
	SUR_MSG_CHR_HDR out;
	btosSI(SUR_CLOSE, out.token, intWidth);
	btosSI(0, out.nbytes, intWidth);
	btosSI(surSpid, out.surPid, intWidth);
#else			/********** binary message **********/
	const int size = sizeof(SUR_MSG_HDR);
	SUR_MSG_HDR out;
	out.token = SUR_CLOSE;
	out.nbytes = 0;
	out.surPid = surSpid;
#endif

// send a message to the rs232 serial writer
if (Send(serialWriter, &out, NULL, size, 0) == -1)
	{
	_simpl_log("%s: send error on clode msg-%s\n", fn, whatsMyError());
	}
}

/**********************************************************************
FUNCTION:	hndlProxy(int)

PURPOSE:	Handle sending a proxy to the remote surrogate_s.

RETURNS:	int	
**********************************************************************/	

int hndlProxy(int proxyValue)
{
// serialWriter is global
// surSpid is global
const char *fn = "hndlProxy_rs232";

#ifdef SUR_CHR	/********** char message **********/
	const int size = sizeof(SUR_CHR_PROXY_MSG);
	SUR_CHR_PROXY_MSG out;
	btosSI(SUR_PROXY, out.hdr.token, intWidth);
	btosSI(surSpid, out.hdr.surPid, intWidth);
	btosSI(sizeof(out.proxyValue), out.hdr.nbytes, intWidth);
	btosSI(proxyValue, out.proxyValue, intWidth);
#else			/********** binary message **********/
	const int size = sizeof(SUR_PROXY_MSG);
	SUR_PROXY_MSG out;
	out.hdr.token = SUR_PROXY;
	out.hdr.nbytes = sizeof(out.proxyValue);
	out.hdr.surPid = surSpid;
	out.proxyValue = proxyValue;
#endif

// send a message to the rs232 serial writer
if (Send(serialWriter, &out, NULL, size, 0) == -1)
	{
	_simpl_log("%s: send error on proxy msg-%s\n", fn, whatsMyError());
	return(-1);
	}

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
