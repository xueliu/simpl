/***********************************************************************

FILE:			surrogate_R.c

DATE:			07 July 19

DESCRIPTION:	This program is run as a fork from surrogate.c

NOTES:			surrogate_R (parent) sends the following messages: 
				1. SUR_PROTOCOL_MSG to protocolRouter

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
$Log: surrogate_R.c,v $
Revision 1.2  2010/01/29 20:40:09  johnfcsoft
zombie/signal

Revision 1.1  2008/04/21 17:22:03  johnfcsoft
initial


***********************************************************************/

// application header
#define _ALLOC extern
#include "surrogate.h"
#undef _ALLOC

/**********************************************************************
FUNCTION:	surrogate_R(void)

PURPOSE:	sends "child" messages to protocol_router.
  
RETURNS:	void
**********************************************************************/	

void surrogate_R()
{
const char *fn = SURROGATE_RS232_R_PARENT;
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
if (name_attach(fn, NULL) == -1)
	{
	_simpl_log("%s: cannot attach name-%s\n", fn, whatsMyError());
	exit(-1);
	}

// name locate the protocol router
pr = name_locate(PROTOCOL_ROUTER);
if (pr == -1)
	{
	_simpl_log("%s :name locate protocol router error-%s\n", fn, whatsMyError());
	exit(-1);
	}

while (1)
	{
	// fork a surrogate receiver child which will wait until needed
	pid = forkSurrogate();
	if (pid < 0)
		{
		_simpl_log("%s: forkSurrogate failure\n", fn);
		exit(-1);
		}
	
	/*
	send pid of latest available surrogate child and become send blocked by the
	protocol router who will unblock when the latest surrogate has been assigned
	*/
	if (sendSurrogateMsg(pr, pid) == -1)
		{
		_simpl_log("%s: sendSurrogateMsg error-%s\n", fn, whatsMyError());
		exit(-1);
		}
	}
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

/**********************************************************************
FUNCTION:	sendSurrogateMsg(int, pid_t)

PURPOSE:	Send a surrogate child message to the protocol router.

RETURNS:	int
**********************************************************************/	

int sendSurrogateMsg(int pr, pid_t pid)
{
const static char *fn = "sendSurrogateMsg";
SUR_PROTOCOL_MSG outMsg;

// build message to protocol router
outMsg.hdr.token = SUR_SURROGATE_READY;
outMsg.hdr.nbytes = sizeof(SUR_PROTOCOL_MSG); 
strcpy(outMsg.protocolName, SIMPL_RS232);
sprintf(outMsg.programName, "%s_%d", SURROGATE_RS232_R_CHILD, pid);

// send name locate message to the protocol router
if (Send(pr, &outMsg, NULL, sizeof(SUR_PROTOCOL_MSG), 0) == -1)
	{
	_simpl_log("%s: send to protocol router error-%s\n", fn, whatsMyError());
	return(-1);
	}

return(0);
}
