/***********************************************************************

FILE:			surrogate_R.c

DATE:			05 Oct 28

DESCRIPTION:	This program is run as a fork from surrogate.c

NOTES:			surrogate_R (parent) receives the following messages:
				1. SUR_REQUEST_PROTOCOL_MSG

				surrogate_R (parent) sends the following messages: 
				1. SUR_PROTOCOL_MSG (de)registering with protocolRouter
				2. SUR_PROTOCOL_REPLY_MSG

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
$Log: surrogate_R.c,v $
Revision 1.4  2010/01/29 20:41:53  johnfcsoft
zombie/signal

Revision 1.3  2007/01/09 15:27:00  bobfcsoft
restore from tcp_x

Revision 1.6  2006/01/26 02:15:18  bobfcsoft
v3.x enhancements

Revision 1.5  2006/01/10 15:32:22  bobfcsoft
v3.0 changes

***********************************************************************/

// application header
#define _ALLOC extern
#include "surrogate.h"
#undef _ALLOC

/**********************************************************************
FUNCTION:	surrogate_R(void)

PURPOSE:	receives "child" messages from protocol_router.
  
RETURNS:	void
**********************************************************************/	

void surrogate_R()
{
// sender is global
// memArea is global
// memSize is global
const char *me = SURROGATE_TCP_R_PARENT;
pid_t pid;
int pr;

/*
The original way to deal with "dead" children was to "wait" them out. Unless
the parent was notified somehow that its child was dead, the child continued
to be in the kernel's process table. Eventually, with enough dead or zombie
children in the process table, no more processes could be started.The ARM
processor has no such vehicle for "waiting". It is now possible to trap a new
signal called SA_NOCLDWAIT allowing the kernel to clear zombie children from its
process list which is supported by ARM but may not be supported by older
systems. Accordingly, the original method for waiting out zombies has been left
in this code and can be activated by comipling with a -D ZOMBIE def.

NOTE: the default is the newer signal trapping method.
*/
#ifdef NO_ZOMBIE
struct sigaction zom;
zom.sa_handler = SIG_IGN;
sigemptyset(&zom.sa_mask);
zom.sa_flags = SA_NOCLDWAIT;
sigaction(SIGCHLD, &zom, NULL);
#endif

// attach a name for local IPC
if (name_attach(me, NULL) == -1)
	{
	_simpl_log("cannot attach name-%s\n", whatsMyError());
	exit(-1);
	}

// name locate the protocol router
pr = name_locate(PROTOCOL_ROUTER);
if (pr == -1)
	{
	_simpl_log("name locate protocol router error-%s\n", whatsMyError());
	exit(-1);
	}

// global message memory for parent, set to largest message encountered
if (adjustMemory(sizeof(SUR_PROTOCOL_MSG)) == -1)
	{
	_simpl_log("memory allocation error-%s\n", strerror(errno));
	exit(-1);
	}

while (1)
	{
	// fork a surrogate receiver child which will wait until needed
	pid = forkSurrogate();
	if (pid < 0)
		{
		_simpl_log("forkSurrogate failure\n");
		exit(-1);
		}
	
	/*
	send pid of latest available surrogate child and become send blocked by the
	protocol router who will unblock when the latest surrogate has been assigned
	*/
	if (sendSurrogateMsg(pr, pid) == -1)
		{
		_simpl_log("sendSurrogateMsg error-%s\n", whatsMyError());
		exit(-1);
		}
	}
}
		
/**********************************************************************
FUNCTION:	sendSurrogateMsg(int, pid_t)

PURPOSE:	Send a surrogate child message to the protocol router.

RETURNS:	int
**********************************************************************/	

int sendSurrogateMsg(int pr, pid_t pid)
{
// memArea is global 
const static char *fn = "sendSurrogateMsg";
SUR_PROTOCOL_MSG *outMsg;

// build message to protocol router
outMsg = (SUR_PROTOCOL_MSG *)memArea;
outMsg->hdr.token = SUR_SURROGATE_READY;
outMsg->hdr.nbytes = sizeof(SUR_PROTOCOL_MSG); 
strcpy(outMsg->protocolName, SIMPL_TCP);
sprintf(outMsg->programName, "%s_%d", SURROGATE_TCP_R_CHILD, pid);

// send name locate message to the protocol router
if (Send(pr, outMsg, NULL, sizeof(SUR_PROTOCOL_MSG), 0) == -1)
	{
	_simpl_log("%s: send to protocol router error-%s\n", fn, whatsMyError());
	return(-1);
	}

return(0);
}

/**********************************************************************
FUNCTION:	forkSurrogate(void)

PURPOSE:	Fork a surrogate receiver if possible.

RETURNS:	pid_t to the parent process	
**********************************************************************/	

pid_t forkSurrogate()
{
const static char *fn = "forkSurrogate";
pid_t pid;

// fork child (surrogate) to handle messaging
pid = fork();
if (pid < 0) // fork failure
	{
	_simpl_log("%s: fork error-%s\n", fn, strerror(errno));
	}
else if (pid == 0) // child
	{
	surrogate_r();
	}
else // parent
	{
	#ifdef ZOMBIE
	// remove any dead children
	killZombies();
	#endif
	}

// in the case of the parent
return(pid);
}
