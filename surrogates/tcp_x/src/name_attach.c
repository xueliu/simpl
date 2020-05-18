/***********************************************************************

FILE:			name_attach.c

this code needs to migrate to the simplipc core library

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

***********************************************************************/

// application header
#define _ALLOC extern
#include "surrogate.h"
#undef _ALLOC


/**********************************************************************
FUNCTION:	int rname_attach(const char *, void(*myExit)())

PURPOSE:	Initializes SIMPL module

RETURNS:	success: >= 0
			failure: -1
***********************************************************************/

int rname_attach(const char *myName, void (*myExit)())
{
const static char *fn = "name_attach";
register int i;
char *p;
// WHO_AM_I _simpl_myStuff is global
// char *_simpl_fifoPath is global
// int _simpl_remoteReceiverId[] is global
// char *_simpl_blockedSenderId[] is global

// check minimal length of the process name
if (!strlen(myName))
	{
	_simpl_setErrorCode(NO_NAME);
	return(-1);
	}

#if 0
// set the global fifo path
// default to DEFAULT_FIFO_PATH if FIFO_PATH not defined
p = getenv("FIFO_PATH");

if (p == NULL)
	sprintf(_simpl_fifoPath,DEFAULT_FIFO_PATH);
else
	{
	if (strlen(p) > MAX_FIFO_PATH_LEN)
		{
		_simpl_setErrorCode(FIFO_PATH_TOO_LONG);
		_simpl_log("%s: FIFO_PATH too long\n", fn);
		return(-1);
		}
	sprintf(_simpl_fifoPath,"%s",p);
	}
	
if (access(_simpl_fifoPath,F_OK) == -1)
	{
	_simpl_setErrorCode(NO_FIFO_PATH);
	_simpl_log("%s: no fifo path defined\n", fn);
	return(-1);
	}

// for checking purposes
_simpl_myStuff.pid = getpid();
#endif


// store the name and truncate it if necessary
sprintf(_simpl_myStuff.whom, "%.*s", MAX_PROGRAM_NAME_LEN, myName);
	
// check whether name is already in use
if (_simpl_statFifoName(_simpl_myStuff.whom, REMOVE) != -1)
	{
	_simpl_setErrorCode(NAME_NOT_AVAILABLE);
	return(-1);
	}

// name, create and open the receive fifo for a sender to kick message
_simpl_myStuff.fd = _simpl_initFifo(RECEIVE);
if (_simpl_myStuff.fd == -1)
	{
	return(-1);
	}

// name, create and open the receive fifo for a sender to kick message
_simpl_myStuff.y_fd = _simpl_initFifo(REPLY);
if (_simpl_myStuff.y_fd == -1)
	{
	return(-1);
	}

// set shmem size to indicate that shmem has not yet been made
_simpl_myStuff.shmSize = 0;

// store the user exit function
_simpl_myStuff.myExit = myExit;

/*
add a signal handler, _simpl_myStuff.myExit must be added to 
as an atexit() function and the signal handler
*/
_simpl_initSignalHandling();

/*
add the user exit functionality
note that the user exit stuff will be run prior to the internal simpl
stuff because the user may need the simpl capabilities. multiple atexit
calls lierally stack up exit functions in LIFO style such that the last 
lsted is the first run
*/
atexit(_simpl_exitFunc);
if (_simpl_myStuff.myExit != (void*)NULL)
	{
	atexit((void *)_simpl_myStuff.myExit);
	}

// initialize table of possible surrogates
for (i = 0; i < MAX_NUM_REMOTE_RECEIVERS; i++)
	{
	_simpl_remoteReceiverId[i] = -1;
	}

// initialize table of reply-blocked senders
for (i = 0; i < MAX_NUM_BLOCKED_SENDERS; i++)
	{
	_simpl_blockedSenderId[i] = (char *)NULL;
	}

return(0);
} // end name_attach
int rc;
register int i;
char protocolName[MAX_PROTOCOL_NAME_LEN + 1];
char hostName[MAX_HOST_NAME_LEN + 1];
char processName[MAX_PROGRAM_NAME_LEN + 1];
char localHost[MAX_HOST_NAME_LEN + 1];
struct hostent *local;
struct in_addr remote;

// is this program name attached?
if (_simpl_check() == -1)
	{
	_simpl_setErrorCode(NO_NAME_ATTACHED);
	return(-1);
	}

// extract the protocol (if any), host (if any) and process names 
/*
******************************************************************************
_simpl-getNames() nulls out the protocolName, hostName and processName strings
******************************************************************************
*/
if (_simpl_getNames(names, protocolName, hostName, processName) == -1)
	{
	_simpl_setErrorCode(NO_NAME);
	return(-1);
	}

// there must at least be a destination process name
if ( strlen(processName) == 0 )
	{
	_simpl_setErrorCode(NO_NAME_ATTACHED);
	return(-1);
	}

// is this process local or remote?
if ( strlen(hostName) == 0 )
	{
	// no entry in host field implies a local receiver
	rc = _simpl_local_name_locate(processName);
	}
else
	{
	// get the local host's name for later comparisons
	if (_simpl_get_hostname(localHost) == -1)
		{
		_simpl_setErrorCode(NO_SYSTEM_HOSTNAME);
		return(-1);
		}

	// is the destination hostname a name or an ip address?
	// try to convert as dotted decimal address: failure inplies a hostname
	if ( !inet_aton(hostName, &remote) )
		{
		// this is a host name
		if ( !strcmp(hostName, localHost) )
			{
			// destination hostname is local host's name - use local simpl
			rc = _simpl_local_name_locate(processName);
			}
		else
			{
			// a non-local host name implies that the process is remote
			rc = _simpl_net_name_locate(protocolName, hostName, processName);
			}
		}
	else
		{
		// this a dotted ip address
		// get the local host information for comparison purposes
		if(strcmp(hostName, "127.0.0.1") == 0)
			{
			rc = _simpl_net_name_locate(protocolName, hostName, processName);
			}
		else
			{
			local = gethostbyname(localHost);
			if (local == NULL)
				{
				_simpl_setErrorCode(LOCAL_HOST_PROBLEM);
				return(-1);
				}
	
			// look through all ip entries
			for (i = 0; local->h_addr_list[i]; i++)
				{
				// compare remote and local ip adresses
				if ( !strcmp(hostName, inet_ntoa( *(struct in_addr *)
												local->h_addr_list[i])) )
					{
					// destination ip is the local host's ip - use local simpl
					rc = _simpl_local_name_locate(processName);
					return(rc);
					}
				}

			// if we got this far then the ip and the local ip do not match
			// a non-local ip address implies that the process is remote
			rc = _simpl_net_name_locate(protocolName, hostName, processName);
			}
		} // end else 127.0.0.1
	}

return(rc);
}
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

/**********************************************************************
FUNCTION:	nameAttachReply(int)

PURPOSE:	This function reports the results of a remote name_attach()
			to the calling sender surrogate.

RETURNS:	void
**********************************************************************/	

int nameAttachReply(int result)
{
// memArea is global
// wFp is global
const static char *fn = "nameAttachReply";

#ifdef SUR_CHR	/********** char message **********/
	const static int size = sizeof(SUR_NAME_LOCATE_CHR_REPLY);
	SUR_NAME_LOCATE_CHR_REPLY *reply;
#else			/********** binary message **********/
	const static int size = sizeof(SUR_NAME_ATTACH_REPLY);
	SUR_NAME_ATTACH_REPLY *reply;
#endif

// build the reply message
#ifdef SUR_CHR	/********** char message **********/
	reply = (SUR_NAME_LOCATE_CHR_REPLY *)memArea;
	btosSI(SUR_NAME_LOCATE, reply->hdr.token, intWidth);
	btosSI(result, reply->result, intWidth);
#else			/********** binary message **********/
	reply = (SUR_NAME_ATTACH_REPLY *)memArea;
	reply->hdr.token = SUR_NAME_ATTACH;
	reply->hdr.nbytes = sizeof(SUR_NAME_ATTACH_REPLY) - sizeof(SUR_MSG_HDR);
	reply->result = result;
#endif
	
if (surWrite(memArea, size) == -1)
	{
	_simpl_log("%s: write error-%s\n", fn, strerror(errno));
	return(-1);
	}

return(0);
}// end nameAttachReply
