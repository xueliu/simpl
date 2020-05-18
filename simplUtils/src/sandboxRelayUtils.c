/*************************************************************

FILE:		sandboxRelayUtils.c

DESCRIPTION:	
This file contains utilities for sandboxRelay

AUTHOR:			R.D. Findlay

-----------------------------------------------------------------------
    Copyright (C) 2007 SIMPL project. 

    This software is in the public domain.
    Permission to use, copy, modify, and distribute this software and its
    documentation for any purpose and without fee is hereby granted, 
    without any conditions or restrictions.
    This software is provided "as is" without express or implied warranty.

    If you discover a bug or add an enhancement contact us on the SIMPL
    project mailing list. 

-----------------------------------------------------------------------
Revision history:
=======================================================
$Log: sandboxRelayUtils.c,v $
Revision 1.1  2007/07/03 18:28:04  bobfcsoft
added sandboxRelay

=======================================================

*************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

#define _ALLOC extern
#include "sandboxRelay.h"
#undef _ALLOC

extern WHO_AM_I _simpl_myStuff;

#include "sandboxRelayProto.h"
#include "simplLibProto.h"   // for _simpl_check
#include "simplProto.h"
#include "loggerProto.h"
#include "simplmiscProto.h"

/**********************************************************************
FUNCTION:	int sb_name_attach(const char *, void (*myExit)())

PURPOSE:	Initializes required SIMPL functionality. 

RETURNS:	success:  0
			failure: -1
***********************************************************************/

int sb_name_attach(const char *myName)
{
static char *fn = "sb_name_attach";
char name[128];

	fcLogx(__FILE__, fn,
		globalMask,
		TRACE_MARK,
		"myName=<%s>",myName);

// create a reply fifo in other sandbox
sprintf(name, "%s/YSB_%s.%d",	otherFifoPath,
				_simpl_myStuff.whom,
				_simpl_myStuff.pid);
if (mkfifo(name, 0666) == -1)
	{
	fcLogx(__FILE__, fn,
		TRACE_MARK,
		TRACE_MARK,
	"unable to create reply fifo %s-%s", name, strerror(errno));
	return(-1);
	}
sb_fd = -1; // we don't set sb_fd until needed

return(0);
} // end name_attach

/**********************************************************************
FUNCTION:	int sb_name_detach(void)

PURPOSE:	Removes SIMPL functionality upon process exit.

RETURNS:	success: 0
			failure: -1
***********************************************************************/

void sb_name_detach()
{
char name[128];

// close the reply file descriptor in other sandbox
if (sb_fd != -1)
	{
	close(sb_fd);
	sb_fd = -1;
	}

// remove Reply fifo in other sandbox
sprintf(name, "%s/YSB_%s.%d",	otherFifoPath,
				_simpl_myStuff.whom,
				_simpl_myStuff.pid);
remove(name);

} //end sb_name_detach

/**********************************************************************
FUNCTION:	int sb_name_locate(char *)

PURPOSE:	Returns the fd of the receive fifo of a simpl receiver.

RETURNS:	success: >= 0
			failure: -1
***********************************************************************/

int sb_name_locate(char *localName)
{
static char *fn="sb_name_locate";
int rc;

#if 0
// is this program name attached?
if (_simpl_check() == -1)
	{
	fcLogx(__FILE__, fn,
		TRACE_MARK,
		TRACE_MARK,
		"not name attached");

	return(-1);
	}
#endif


fcLogx(__FILE__, fn,
	TRACE_MARK,
	TRACE_MARK,
	"localName=<%s>",localName);

rc = _sb_local_name_locate(localName);

return(rc);
} // end sb_name_locate

/**********************************************************************
FUNCTION:	int _sb_local_name_locate(const char *)

PURPOSE:	Returns the fifo fd of a local simpl receiver.

RETURNS:	success: >= 0
			failure: -1
***********************************************************************/

int _sb_local_name_locate(const char *processName)
{
static char *fn="_sb_local";
char fifoName[128];
int rc;

// find the receiver's fifo
rc = _sb_getFifoName(processName, fifoName);
if (rc == -1)
	{
	fcLogx(__FILE__, fn,
		TRACE_MARK,
		TRACE_MARK,
		"getFifoName failure");

	return(-1);
	} 

fcLogx(__FILE__, fn,
	globalMask,
	TRACE_MARK,
	"fifoName=%s", fifoName);

// open the fifo for triggering message passing
rc = open(fifoName, O_WRONLY);
if (rc == -1)
	{
	fcLogx(__FILE__, fn,
		TRACE_MARK,
		TRACE_MARK,
		"fifo %s open failure", fifoName);

	} 

return(rc);
}


/**********************************************************************
FUNCTION:	int _sb_getFifoName(const char *, char *)

PURPOSE:	Find a receive fifo based on the simpl name.

RETURNS:	success: 0 
			failure: -1
***********************************************************************/

int _sb_getFifoName(const char *simplName, char *fifoName)
{
static char *fn = "_sb_getFifoName";
DIR *directory;
struct dirent *file;
int len;
int ret = -1;
// char *otherFifoPath is global

// open the fifo directory
directory = opendir(otherFifoPath);
if (directory == NULL)
	{
	fcLogx(__FILE__, fn,
		TRACE_MARK,
		TRACE_MARK,
		"cannot open %s", otherFifoPath);

	return(-1);
	}

// the length of the simpl name passed in
len = strlen(simplName);

// check for a match
while ( (file = readdir(directory)) != NULL )
	{
	// check for a match
	if (file->d_name[len] == '.' && !memcmp(file->d_name, simplName, len))
		{
		// simpl names match
		sprintf(fifoName, "%s/%s", otherFifoPath, file->d_name);
		ret = 0;
		break;
		}
	}

closedir(directory);

return(ret);
} // end _sb_getFifoName

/**********************************************************************
FUNCTION:	int sbSend(int, void *, void *, unsigned, unsigned)

PURPOSE:	This function sends simpl messages to other processes.

RETURNS:	success: number of bytes from Reply >= 0
			failure: -1

NOTE:		Cannot use fcLogx calls in mainline code because
		this involves and internal SIMPL Send() which resets
		the shm area used by the Reply()
***********************************************************************/

int sbSend(int fd, void *outBuffer, void *inBuffer, unsigned outBytes, unsigned inBytes)
{
static char *fn = "sbSend";
char fifoBuf[sizeof(FIFO_MSG)];
FIFO_MSG *fifoMsg = (FIFO_MSG *)fifoBuf;
unsigned bufSize;
FCMSG_REC *msgPtr;
// WHO_AM_I _simpl_myStuff is global 

// is this program name attached?
if (_simpl_check() == -1)
	{
	fcLogx(__FILE__, fn,
		TRACE_MARK,
		TRACE_MARK,
		"no name attached");

	return(-1);
	}

// calculate the largest buffer size
bufSize = (outBytes >= inBytes) ? outBytes : inBytes;

// build shmem as needed
if (_simpl_myStuff.shmSize < bufSize)
	{
	// delete any past shmem
	if (_simpl_myStuff.shmSize)
		{
		_simpl_deleteShmem();
		}
	
	// create new shmem	
	if (_simpl_createShmem(bufSize) == -1)
		{
		return(-1);
		}
	}

// copy the message into shmem to be read by the receiver
msgPtr = (FCMSG_REC *)_simpl_myStuff.shmPtr;
sprintf(msgPtr->whom, "SB_%s", _simpl_myStuff.whom);
msgPtr->pid = _simpl_myStuff.pid;
msgPtr->nbytes = outBytes;
msgPtr->ybytes = inBytes;
memcpy((void *)&msgPtr->data, outBuffer, outBytes);

//printf("sb_fd=%d\n", sb_fd);

// open reply fifo if first time
if (sb_fd == -1)
	{
	char fifoName[128];

	sprintf(fifoName, "%s/YSB_%s.%d",	otherFifoPath,
				_simpl_myStuff.whom,
				_simpl_myStuff.pid);

// NOTE:
// Cannot use fcLogx calls here because they
// mess up the shm area associated with this relay
//printf("fifoName=<%s>\n", fifoName);

	sb_fd = open(fifoName, O_RDWR);
	if (sb_fd == -1)
		{
	fcLogx(__FILE__, fn,
		TRACE_MARK,
		TRACE_MARK,
		"unable to open reply fifo %s-%s", fifoName, strerror(errno));

		return(-1);
		} 
	}

// line up the triggering message for the fifo
fifoMsg->shmid = _simpl_myStuff.shmid;

// receiver reads the fifo and then sender's shmem
if (write(fd, fifoBuf, sizeof(FIFO_MSG)) != sizeof(FIFO_MSG))
	{
	fcLogx(__FILE__, fn,
		TRACE_MARK,
		TRACE_MARK,
	"unable to write to fifo -%s", strerror(errno));
	return(-1);
	}

// wait for the receiver to send fifo message to trigger the reply
if (_simpl_readFifoMsg(sb_fd, fifoBuf) != sizeof(FIFO_MSG))
	{
	fcLogx(__FILE__, fn,
		TRACE_MARK,
		TRACE_MARK,
	"unable to read from fifo");
	close(sb_fd);
	sb_fd = -1;
	return(-1);
	}

// was there a problem in the send?
if (fifoMsg->shmid == -1)
	{
	fcLogx(__FILE__, fn,
		TRACE_MARK,
		TRACE_MARK,
	"Receive/Reply problem");
	return(-1);
	}

if (inBuffer != NULL)
	{
	// copy the reply message
	if (msgPtr->nbytes)
		{ 
		memcpy(inBuffer, (void *)&msgPtr->data, msgPtr->nbytes);
		}
	}

// return the sizeof the reply message
return(msgPtr->nbytes);
}// end sbSend

