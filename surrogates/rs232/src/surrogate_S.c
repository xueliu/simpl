/***********************************************************************

FILE:			surrogate_S.c

DATE:			07 July 23

DESCRIPTION:	This program runs as a fork from surrogate.c
 
NOTES:			surrogate_S (parent) receives the following messages:
				1. SUR_NAME_LOCATE_TEXT_MSG/SUR_NAME_LOCATE_MSG

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
$Log: surrogate_S.c,v $
Revision 1.2  2010/01/29 20:40:24  johnfcsoft
zombie/signal

Revision 1.1  2008/04/21 17:23:40  johnfcsoft
initial


***********************************************************************/

// application header
#define _ALLOC extern
#include "surrogate.h"
#undef _ALLOC

/**********************************************************************
FUNCTION:	surrogate_S()

PURPOSE:	Acts as a receiver for remote name locate messages.

RETURNS:	void
**********************************************************************/	

void surrogate_S()
{
// inMsgArea is global
// serialWriter is global
char *fn = SURROGATE_RS232_S_PARENT;
int token;
pid_t childPid;
char *sender;

#ifdef SUR_CHR	/********** char message **********/
	const static int size = sizeof(SUR_NAME_LOCATE_CHR_MSG);
	SUR_MSG_CHR_HDR *hdr;
#else			/********** binary message **********/
	const static int size = sizeof(SUR_NAME_LOCATE_MSG);
	SUR_MSG_HDR *hdr;
#endif 

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

// global message memory for parent, set to largest message encountered
if (checkMemory(IN, size) == -1)
	{
	_simpl_log("%s: check memory error-%s\n", fn, strerror(errno));
	exit(-1);
	}

while (1)
	{
	// read the message, should be a SUR_NAME_LOCATE_*_MSG from the rs232_r program
	if (Receive(&sender, inMsgArea, size) == -1)
		{
		_simpl_log("%s: receive error-%s\n",fn, whatsMyError());
		exit(-1);
		}

	// an empty reply to the serial reader
	if (Reply(sender, NULL, 0) == -1)
		{
		_simpl_log("%s: reply error-%s\n", fn, whatsMyError());
		exit(-1);
		}

	// interpret message header
	#ifdef SUR_CHR	/********** char message **********/
		hdr = (SUR_MSG_CHR_HDR *)inMsgArea;
		token = stobSI(hdr->token, intWidth);
	#else			/********** binary message **********/
		hdr = (SUR_MSG_HDR *)inMsgArea;
		token = hdr->token;
	#endif

	if (token == SUR_NAME_LOCATE)
		{
		// fork child (surrogate) to handle messaging
		childPid = fork();
		if (childPid < 0) // forking failure
			{
			_simpl_log("%s: fork error-%s\n", fn, strerror(errno));
			exit(-1);
			}
		else if (childPid == 0) // child
			{
			// child will have original message in its copy of inMsgArea
			surrogate_s();
			}
		}
	else
		{
		_simpl_log("%s: unknown msg token=%d\n", fn, token);
		}

	#ifdef ZOMBIE
	// check for zombie children
	killZombies();
	#endif
	}
}
