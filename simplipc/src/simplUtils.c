/*======================================================================

FILE:			simplUtils.c

DESCRIPTION:	This file contains the function calls used by the 
				simpl library functions.

NOTES:			The functions are gathered into related groups.

AUTHOR:			FC Software Inc.

-----------------------------------------------------------------------
    Copyright (C) 2000 FCSoftware Inc. 

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    If you discover a bug or add an enhancement contact us on
    the SIMPL project mailing list. 

-----------------------------------------------------------------------

Revision history:
====================================================================
$Log: simplUtils.c,v $
Revision 1.20  2012/10/30 19:50:53  bobfcsoft
added CYGWIN hooks

Revision 1.19  2011/09/08 16:21:42  bobfcsoft
bug fixes in name_locate logic

Revision 1.18  2011/03/23 17:23:35  bobfcsoft
added remote name attach hooks

Revision 1.17  2010/04/16 11:56:15  bobfcsoft
use of SIMPL_NAME_DELIMITER in prep for IPv6 code

Revision 1.16  2010/01/08 21:08:57  bobfcsoft
using _SC_PAGESIZE

Revision 1.15  2009/03/16 15:50:23  johnfcsoft
cleanup

Revision 1.14  2009/01/15 16:44:06  bobfcsoft
forced a+rw on fifo creation

Revision 1.13  2009/01/13 20:46:56  johnfcsoft
added init/destroy fifo funcs

Revision 1.12  2009/01/12 19:55:08  johnfcsoft
cleanup

Revision 1.11  2009/01/12 19:48:33  johnfcsoft
added statFifoName to local_name_locate

Revision 1.10  2008/12/17 15:53:13  bobfcsoft
replaced _simpl_deleteShmem with _simpl_detachShmem

Revision 1.9  2008/04/21 13:44:26  johnfcsoft
added shmem release

Revision 1.8  2007/07/24 19:31:10  bobfcsoft
new contact info

Revision 1.7  2007/07/24 19:29:15  bobfcsoft
commented out unneccessary postMsg code block

Revision 1.6  2006/04/24 22:12:06  bobfcsoft
fixed nbytes to subtract header length

Revision 1.5  2006/01/26 02:13:42  bobfcsoft
v3.0 enhancements

Revision 1.4  2006/01/10 15:29:40  bobfcsoft
v3.0 changes

Revision 1.3  2005/11/12 12:21:34  bobfcsoft
added postMsg/readReply funcs

Revision 1.2  2005/09/26 15:47:22  bobfcsoft
proxy/trigger changes

Revision 1.1.1.1  2005/03/27 11:50:54  paul_c
Initial import

Revision 1.16  2004/10/28 12:44:23  root
fixed another instance of SIMPL name compare
where subset of a name is common

Revision 1.15  2004/07/13 16:43:29  root
write side of fifo is O_WRONLY
and SIGPIPE is intercepted and ignored

Revision 1.14  2004/05/06 15:36:07  root
fixed bug in _simpl_statFifoName which
prevented the duplicate name detection from working properly

Revision 1.13  2003/12/08 16:12:08  root
created _simpl_detachShmem function

Revision 1.12  2003/07/17 16:04:16  root
fixed BOBT and BOBTCL name_attach refusals on names which are part of
other SIMPL names eg.  BOBT and BOBTCL

Revision 1.11  2003/06/23 16:16:59  root
added simplErrors.h

Revision 1.10  2003/04/14 13:41:44  root
enabled MAC_OS_X stuff

Revision 1.9  2002/11/22 16:36:01  root
2.0rc3

======================================================================*/

#ifdef _MAC_OS_X
	#include <db.h>
	#include <sys/time.h>
	#define PAGE_SIZE 1024
#endif

// system headers
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/shm.h>
#include <sys/stat.h>
//#include <sys/user.h>
#ifdef _CYGWIN
#include <sys/select.h>
#endif
#ifdef _EXPMTL
#include <sys/mman.h>
#endif

// simpl headers 
#include <simplDefs.h>
#include <simplProto.h>
#include <simplLibProto.h>
#include <simplProtocols.h>
#include <simplNames.h>
#include <surMsgs.h>

#define _ALLOC extern
#include <simplLibVars.h>
#include <simplErrors.h>
#undef _ALLOC

/********************************************************************/
/*********************** NAME LOCATE FUNCTIONS **********************/
/********************************************************************/

/**********************************************************************
FUNCTION:	int _simpl_getNames(char *, char *, char *)

PURPOSE:	extract the protocol, host and process names from the colon 
			delimited input name.

RETURNS:	success: 0
			failure: -1

NOTES:		Function truncates protocol, host and process names if they
			are too long.
**********************************************************************/	

int _simpl_getNames(const char *names, char *protocolName, char *hostName, char
*processName)
{
register int i;
int totalLen;
int numColons = 0;
int colons[2];

// initialize the name strings
memset(protocolName, 0, MAX_PROTOCOL_NAME_LEN + 1);
memset(hostName, 0, MAX_HOST_NAME_LEN + 1);
memset(processName, 0, MAX_PROGRAM_NAME_LEN + 1);

// is there a name to process?
totalLen = strlen(names);
if (!totalLen)
	{
	_simpl_setErrorCode(NAME_TOO_SHORT);
	return(-1);
	}

// look for colons in the names string 
for (i = 1; i <= totalLen; i++)
	{
	if (names[i - 1] == SIMPL_NAME_DELIMITER)
		{
		if (numColons < 2)
			{
			colons[numColons] = i;
			}
		numColons++;
		}
	} 	

// logic is based on number of delimiting colons found
if (numColons == 0)
	{
	/*
	this is interpreted as a name_locate() call on a local process
	so the entire string "names" is the local process' name
	*/

	if (totalLen <= MAX_PROGRAM_NAME_LEN)
		{
		// process name length ok
		memcpy(processName, names, totalLen);
		}
	else
		{
		// truncate the process name length
		memcpy(processName, names, MAX_PROGRAM_NAME_LEN);
		}
	}
else if (numColons == 1)
	{
	/*
	this is interpreted as a name_locate() call on a remote process
	using the default protocol
	*/

	// set the protocol name
	strcpy(protocolName, SIMPL_DEFAULT);

	// set the host name 
	if ( (colons[0] - 1) <= MAX_HOST_NAME_LEN)
		{
		memcpy(hostName, names, colons[0] - 1);
		}
	else
		{
		memcpy(hostName, names, MAX_HOST_NAME_LEN);
		}

	// set the process name
	if ( (totalLen - colons[0]) <= MAX_PROGRAM_NAME_LEN)
		{
		memcpy(processName, names + colons[0], totalLen - colons[0]);
		}
	else
		{
		memcpy(processName, names + colons[0], MAX_PROGRAM_NAME_LEN);
		}
	}
else if (numColons == 2)
	{
	/*
	this is interpreted as a name_locate() call on a remote process
	using a the default protocol
	*/

	if ( (colons[0] - 1) == 0)
		{
		strcpy(protocolName, SIMPL_DEFAULT);
		}
	else
		{
		// set the protocol name
		if ( (colons[0] - 1) <= MAX_PROTOCOL_NAME_LEN)
			{
			memcpy(protocolName, names, colons[0] - 1);
			}
		else
			{
			memcpy(protocolName, names, MAX_PROTOCOL_NAME_LEN);
			}
		}

	// set the host name 
	if ( (colons[1] - colons[0] - 1) <= MAX_HOST_NAME_LEN)
		{
		memcpy(hostName, names + colons[0], colons[1] - colons[0] - 1);
		}
	else
		{
		memcpy(hostName, names + colons[0], MAX_HOST_NAME_LEN);
		}

	// set the process name
	if ( (totalLen - colons[1]) <= MAX_PROGRAM_NAME_LEN)
		{
		memcpy(processName, names + colons[1], totalLen - colons[1]);
		}
	else
		{
		memcpy(processName, names + colons[1], MAX_PROGRAM_NAME_LEN);
		}
	}
else
	{
	/*
	too many colons in the string "name"
	*/
	_simpl_setErrorCode(TOO_MANY_COLONS);
	return(-1);
	}

return(0); 			
}

/**********************************************************************
FUNCTION:	int _simpl_get_hostname(char *)

PURPOSE:	Gets the local host's name.

RETURNS:	success: = 0
			failure: -1
***********************************************************************/

int _simpl_get_hostname(char *name)
{
char *host;
int rc = 0;
int fd;
int ch;
const char *hostFile = "/etc/hostname";
const char *environVar = "HOSTNAME";

// initialize potential local host name
memset(name, 0, MAX_HOST_NAME_LEN + 1);

#ifdef _CYGWIN
rc=gethostname(name, MAX_HOST_NAME_LEN);
if(rc == -1)
	{
	_simpl_setErrorCode(NO_SYSTEM_HOSTNAME);
	}
#else

// get the host name from an environment variable
host = getenv(environVar);
if (host == NULL)
	{
	// look up the hostname in the /etc/hostname file
	fd =open(hostFile, O_RDONLY);
	if (fd == -1)
		{
		rc = -1;
		_simpl_setErrorCode(NO_SYSTEM_HOSTNAME);
		}
	else
		{
		ch = read(fd, name, MAX_HOST_NAME_LEN);
		if (ch <= 0)
			{
			rc = -1;
			_simpl_setErrorCode(NO_SYSTEM_HOSTNAME);
			}

		// get rid of the EOF
		if(name[ch-1] == 0xa)
			name[ch-1] = 0;
		
		close(fd);
		
		}
	}
else
	{
	if (strlen(host) > MAX_HOST_NAME_LEN)
		{
		rc = -1;
		_simpl_setErrorCode(SYSTEM_HOSTNAME_TOO_LONG);
		}
	else
		{
		strcpy(name, host);
		}
	}
#endif

return(rc);
}

/**********************************************************************
FUNCTION:	int _simpl_local_name_locate(const char *)

PURPOSE:	Returns the fifo fd of a local simpl receiver.

RETURNS:	success: >= 0
			failure: -1
***********************************************************************/

int _simpl_local_name_locate(const char *processName)
{
const static char *fn = "_simpl_local_name_locate";
int rc;
int i;

// try the local processName first
rc=_simpl_name_locate(processName);
if (rc == -1)
	{
	char theprocess[32];
	
	sprintf(theprocess,"_%s", processName);
// try the _local processName next
	rc=_simpl_name_locate(theprocess);
	if (rc != -1)
		{
		int trigger=-PROXY_CLONE;
		if (write(rc,&trigger, sizeof(int)) != sizeof(int))
			{
			_simpl_setErrorCode(FIFO_WRITE_FAILURE);
			rc = -1;
			}

// add to the remote receiver offset table
		for (i = 0; i < MAX_NUM_REMOTE_RECEIVERS; i++)
			{
			if (_simpl_remoteReceiverId[i] == -1)
				{
				_simpl_remoteReceiverId[i] = rc;
				break;
				}
			}

// the above loop didn't break ==> no room on table
		if (i == MAX_NUM_REMOTE_RECEIVERS)
			{
			_simpl_setErrorCode(TABLE_FULL);
			_simpl_log("%s: no room on the remote receiver table\n", fn);
			return(-1);
			}

		}
	}

return(rc);
}

/**********************************************************************
FUNCTION:	int _simpl_name_locate(const char *)

PURPOSE:	Returns the fifo fd of a local simpl receiver.

RETURNS:	success: >= 0
			failure: -1
***********************************************************************/

int _simpl_name_locate(const char *processName)
{
char fifoName[128];
int rc;

// find the receiver's fifo
rc = _simpl_getFifoName(processName, fifoName);
if (rc == -1)
	{
	_simpl_setErrorCode(FIFO_GET_FAILURE);
	return(-1);
	}

// check whether there is an orphaned fifo
if (_simpl_statFifoName(processName, NO_REMOVE) == -1)
	{
	_simpl_setErrorCode(ORPHANED_FIFO);
	return(-1);
	}

// open the fifo for triggering message passing
rc = open(fifoName, O_WRONLY);
if (rc == -1)
	{
	_simpl_setErrorCode(FIFO_OPEN_FAILURE);
	} 

return(rc);
}

/**********************************************************************
FUNCTION:	int _simpl_net_name_locate(const char *, const char *, const char *)

PURPOSE:	Returns the fifo fd of a local simpl surrogate receiver.

RETURNS:	success: >= 0
			failure: -1
***********************************************************************/

int _simpl_net_name_locate(const char *protocolName, const char *hostName, const char *processName)
{
const static char *fn = "_simpl_net_name_locate";
register int i;
SUR_REQUEST_PROTOCOL_MSG outMsg;
SUR_PROTOCOL_REPLY_MSG inMsg;
SUR_NAME_LOCATE_MSG out;
SUR_NAME_LOCATE_REPLY in;
int rc; 
// int _simpl_remoteReceiverId[] is global

/*
protocolRouter program  will return the name of the required
protocol surrogate child via the protocol parent
*/
rc = _simpl_local_name_locate(PROTOCOL_ROUTER);
if (rc == -1)
	{
	_simpl_log("%s: cannot locate local protocol router-%s\n", fn, whatsMyError());
	return(-1);
	}

// build message to protocol router
outMsg.hdr.token = SUR_REQUEST_PROTOCOL;
outMsg.hdr.nbytes = sizeof(SUR_REQUEST_PROTOCOL_MSG); 
strcpy(outMsg.protocolName, protocolName);

// ask the protocol router for a surrogate
if (Send(rc, &outMsg, &inMsg, sizeof(SUR_REQUEST_PROTOCOL_MSG), sizeof(SUR_PROTOCOL_REPLY_MSG)) == -1)
	{
	close(rc);
	_simpl_log("%s: cannot send to protocol router-%s\n", fn, whatsMyError());
	return(-1);
	}

// close the fd to protocol router immediately
close(rc);

/*
check the reply from the protocol router, if no protocol surrogate
is available, for whatever reason, the programName will be all nulled
out
*/
if (strlen(inMsg.programName) == 0)
	{
	_simpl_setErrorCode(PROTOCOL_NOT_AVAILABLE);
	return(-1);
	}

/*
ask the appropriate surrogate protocol program to find the intended
remote program known as processName on the host with hostName
*/
rc = _simpl_local_name_locate(inMsg.programName);
if (rc == -1)
	{
	_simpl_log("%s: cannot locate %s-%s\n", fn, inMsg.programName, whatsMyError());
	return(-1);
	}

// build name locate message for the protocol surrogate
out.hdr.token = SUR_NAME_LOCATE;
out.hdr.nbytes = sizeof(SUR_NAME_LOCATE_MSG) - sizeof(SUR_MSG_HDR); 
strcpy(out.rHostName, hostName);
strcpy(out.rProgramName, processName);
memset(out.sHostName, 0, MAX_HOST_NAME_LEN + 1);
if (gethostname(out.sHostName, MAX_HOST_NAME_LEN) == -1)
	{
	close(rc);
	_simpl_setErrorCode(HOST_NAME_FAILURE);
	_simpl_log("%s: host name lookup error-%s\n", fn, strerror(errno));
	return(-1);
	}

// send name locate message to local protocol surrogate
if (Send(rc, &out, &in, sizeof(SUR_NAME_LOCATE_MSG), sizeof(SUR_NAME_LOCATE_REPLY)) == -1)
	{
	close(rc);
	_simpl_log("%s: failed send to %s-%s\n", fn, inMsg.programName, whatsMyError());
	return(-1);
	}

if (in.result == -1) // surrogate failed/no process on other box
	{
	_simpl_setErrorCode(NO_NAMED_PROCESS_RUNNING);
	close(rc);
	return(-1);
	}

// add to the remote receiver offset table
for (i = 0; i < MAX_NUM_REMOTE_RECEIVERS; i++)
	{
	if (_simpl_remoteReceiverId[i] == -1)
		{
		_simpl_remoteReceiverId[i] = rc;
		break;
		}
	}

// the above loop didn't break ==> no room on table
if (i == MAX_NUM_REMOTE_RECEIVERS)
	{
	_simpl_setErrorCode(TABLE_FULL);
	_simpl_log("%s: no room on the remote receiver table\n", fn);
	return(-1);
	}

return(rc);
}

/**********************************************************************
FUNCTION:	int _simpl_local_name_attach(const char *, void(*myExit())

PURPOSE:	Returns the fifo fd of a local simpl receiver.

RETURNS:	success: >= 0
			failure: -1
***********************************************************************/

int _simpl_local_name_attach(const char *processName, void (*myExit)())
{
int i;

// store the name and truncate it if necessary
sprintf(_simpl_myStuff.whom, "%.*s", MAX_PROGRAM_NAME_LEN, processName);
	
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
} // end _simpl_local_name_attach

/**********************************************************************
FUNCTION:	int _simpl_net_name_attach(const char *, const char *, const char *)

PURPOSE:	Returns the fifo fd of a local simpl surrogate receiver.

RETURNS:	success: >= 0
			failure: -1
***********************************************************************/

int _simpl_net_name_attach(const char *protocolName, const char *hostName, const char *processName, const char *myName)
{
const static char *fn = "_simpl_net_name_attach";
register int i;
SUR_REQUEST_PROTOCOL_MSG outMsg;
SUR_PROTOCOL_REPLY_MSG inMsg;
SUR_NAME_ATTACH_MSG out;
SUR_NAME_ATTACH_REPLY in;
int rc; 
// int _simpl_remoteReceiverId[] is global

/*
protocolRouter program  will return the name of the required
protocol surrogate child via the protocol parent
*/
rc = _simpl_local_name_locate(PROTOCOL_ROUTER);
if (rc == -1)
	{
	_simpl_log("%s: cannot locate local protocol router-%s\n", fn, whatsMyError());
	return(-1);
	}

// build message to protocol router
outMsg.hdr.token = SUR_REQUEST_PROTOCOL;
outMsg.hdr.nbytes = sizeof(SUR_REQUEST_PROTOCOL_MSG); 
strcpy(outMsg.protocolName, protocolName);

// ask the protocol router for a surrogate
if (Send(rc, &outMsg, &inMsg, sizeof(SUR_REQUEST_PROTOCOL_MSG), sizeof(SUR_PROTOCOL_REPLY_MSG)) == -1)
	{
	close(rc);
	_simpl_log("%s: cannot send to protocol router-%s\n", fn, whatsMyError());
	return(-1);
	}

// close the fd to protocol router immediately
close(rc);

/*
check the reply from the protocol router, if no protocol surrogate
is available, for whatever reason, the programName will be all nulled
out
*/
if (strlen(inMsg.programName) == 0)
	{
	_simpl_setErrorCode(PROTOCOL_NOT_AVAILABLE);
	return(-1);
	}

/*
ask the appropriate surrogate protocol program to setup a 
surrogate pair with remote end known as _processName
*/

rc = _simpl_local_name_locate(inMsg.programName);
if (rc == -1)
	{
	_simpl_log("%s: cannot locate %s-%s\n", fn, inMsg.programName, whatsMyError());
	return(-1);
	}

// build name attach message for the protocol surrogate
out.hdr.token = SUR_NAME_ATTACH;
out.hdr.nbytes = sizeof(SUR_NAME_ATTACH_MSG) - sizeof(SUR_MSG_HDR); 
strcpy(out.rHostName, hostName);
strcpy(out.rProgramName, processName);
strcpy(out.lProgramName, myName);

// send name attach message to local protocol surrogate
if (Send(rc, &out, &in, sizeof(SUR_NAME_ATTACH_MSG), sizeof(SUR_NAME_ATTACH_REPLY)) == -1)
	{
	close(rc);
	_simpl_log("%s: failed send to %s-%s\n", fn, inMsg.programName, whatsMyError());
	return(-1);
	}

if (in.result == -1) // surrogate failed/no process on other box
	{
	_simpl_setErrorCode(NO_NAMED_PROCESS_RUNNING);
	close(rc);
	return(-1);
	}

// add to the remote receiver offset table
for (i = 0; i < MAX_NUM_REMOTE_RECEIVERS; i++)
	{
	if (_simpl_remoteReceiverId[i] == -1)
		{
		_simpl_remoteReceiverId[i] = rc;
		break;
		}
	}

// the above loop didn't break ==> no room on table
if (i == MAX_NUM_REMOTE_RECEIVERS)
	{
	_simpl_setErrorCode(TABLE_FULL);
	_simpl_log("%s: no room on the remote receiver table\n", fn);
	return(-1);
	}

return(rc);
}// end _simpl_net_name_attach


/********************************************************************/
/****************** MESSAGE SHARED MEMORY FUNCTIONS *****************/
/********************************************************************/

/**********************************************************************
FUNCTION:	int _simpl_createShmem(unsigned)

PURPOSE:	Create and attach shared memory used for a message passing
			buffer.

RETURNS:	success: 0
			failure: -1
**********************************************************************/

int _simpl_createShmem(unsigned bufSize)
{
const char *fn = "_simpl_createShmem";
unsigned memSize;
register int i;
long pagesize;
// WHO_AM_I _simpl_myStuff is global

// calculate the shmem size based on the PAGE_SIZE
pagesize=sysconf(_SC_PAGESIZE);
// note that the loop limit is merely to prevent runaway
for (memSize = pagesize, i = 1; i <= 10000; i++, memSize += pagesize)
	{
	if ( (bufSize + sizeof(FCMSG_REC)) <= memSize )
		{
		break;
		}
	}

// create shmem for messages
#ifdef _EXPMTL
_simpl_myStuff.shmid = px_shmget(IPC_PRIVATE, memSize, 0777 | IPC_CREAT);
#else
_simpl_myStuff.shmid = shmget(IPC_PRIVATE, memSize, 0777 | IPC_CREAT);
#endif
if (_simpl_myStuff.shmid == -1)
	{
	_simpl_setErrorCode(CANNOT_CREATE_SHMEM);
	_simpl_log("%s: unable to create shmem-%s\n", fn, strerror(errno));
	return(-1);
	}

// attach the shmem for messages to this process
#ifdef _EXPMTL
_simpl_myStuff.shmPtr = px_shmat(_simpl_myStuff.shmid, 0, 0);
#else
_simpl_myStuff.shmPtr = shmat(_simpl_myStuff.shmid, 0, 0);
#endif
if (_simpl_myStuff.shmPtr == (char *)-1)
	{
	_simpl_setErrorCode(CANNOT_ATTACH_SHMEM);
	_simpl_log("%s: unable to attach shmem to process-%s\n", fn, strerror(errno));
	return(-1);
	}

#ifndef _EXPMTL
#ifdef _LINUX_OS
// release shmem if and when owner dies by an untrappable signal
shmctl(_simpl_myStuff.shmid, IPC_RMID, 0);
#endif
#endif

// this the maximum size for a message
_simpl_myStuff.shmSize = memSize;

return(0);
}

/**********************************************************************
FUNCTION:	int _simpl_detachShmem(void)

PURPOSE:	Detach shared memory used for a message passing
			buffer.

RETURNS:	success: 0
			failure: -1
**********************************************************************/	

int _simpl_detachShmem()
{
const char *fn = "_simpl_detachShmem";
// WHO_AM_I _simpl_myStuff is global

#ifndef _EXPMTL
#if defined(_MAC_OS_X) || defined(_CYGWIN)
// release shmem if and when owner dies by an untrappable signal
shmctl(_simpl_myStuff.shmid, IPC_RMID, 0);
#endif
#endif

// detach shmem
#ifdef _EXPMTL
if (px_shmdt(_simpl_myStuff.shmPtr) == -1)
#else
if (shmdt(_simpl_myStuff.shmPtr) == -1)
#endif
	{
	_simpl_setErrorCode(CANNOT_DETACH_SHMEM);
	_simpl_log("%s: unable to detach shmem to process-%s\n", fn, strerror(errno));
	return(-1);
	}

return(0);
} // end _simpl_detachShmem

/**********************************************************************
FUNCTION:	int _simpl_deleteShmem(void)

PURPOSE:	Detach and delete shared memory used for a message passing
			buffer.

RETURNS:	success: 0
			failure: -1
**********************************************************************/	

int _simpl_deleteShmem()
{
const char *fn = "_simpl_deleteShmem";
// WHO_AM_I _simpl_myStuff is global

// detach shmem
#ifdef _EXPMTL
if (px_shmdt(_simpl_myStuff.shmPtr) == -1)
#else
if (shmdt(_simpl_myStuff.shmPtr) == -1)
#endif
	{
	_simpl_setErrorCode(CANNOT_DETACH_SHMEM);
	_simpl_log("%s: unable to detach shmem to process-%s\n", fn, strerror(errno));
	return(-1);
	}

#ifndef _EXPMTL
// flag shmem removal, removal fails if anyone is still attached 
if (shmctl(_simpl_myStuff.shmid, IPC_RMID, NULL) == -1)
	{
	_simpl_setErrorCode(CANNOT_DELETE_SHMEM);
	_simpl_log("%s: unable to delete shmem-%s\n", fn, strerror(errno));
	return(-1);
	}
#endif

// this the maximum size for a message
_simpl_myStuff.shmSize = 0;

return(0);
}

/********************************************************************/
/************************* FIFO FUNCTIONS ***************************/
/********************************************************************/

/**********************************************************************
FUNCTION:	int _simpl_initFifo(int)

PURPOSE:	Name, create and open receive/reply fifos.

RETURNS:	success (valid fifo fd): >=0
			failure (no valid fifo): -1

***********************************************************************/

int _simpl_initFifo(int type)
{
const char *fn = "_simpl_createFifo";
char name[128];
int fd = -1;
// WHO_AM_I _simpl_myStuff is global; populated in name_attach()
// char *_simpl_fifoPath is global; set early on in name_attach()

if (type == RECEIVE)
	{
	// name receive fifo
	sprintf(name, "%s/%s.%d",	_simpl_fifoPath,
								_simpl_myStuff.whom,
								_simpl_myStuff.pid);
	}
else if (type == REPLY)
	{
	// name reply fifo
	sprintf(name, "%s/Y%s.%d",	_simpl_fifoPath,
								_simpl_myStuff.whom,
								_simpl_myStuff.pid);
	}
else
	{
	_simpl_setErrorCode(FIFO_NAMING_FAILURE);
	_simpl_log("%s: unable to name fifo\n", fn);
	return(-1);
	}

// create  fifo
if (mkfifo(name, 0666) == -1)
	{
	_simpl_setErrorCode(FIFO_CREATION_FAILURE);
	_simpl_log("%s: unable to create fifo %s-%s\n", fn, name, strerror(errno));
	return(-1);
	}

// mkfifo masks the mode 0666 with user's umask
if (chmod(name, 0666) == -1)
	{
	_simpl_setErrorCode(FIFO_CHMOD_FAILURE);
	_simpl_log("%s: unable to chmod fifo %s-%s\n", fn, name, strerror(errno));
	return(-1);
	}

// open  fifo
#ifdef _CYGWIN
fd = open(name, O_RDWR | O_NONBLOCK);
#else
fd = open(name, O_RDWR);
#endif
if (fd == -1)
	{
	_simpl_setErrorCode(FIFO_OPEN_FAILURE);
	_simpl_log("%s: unable to open fifo %s-%s\n", fn, name, strerror(errno));
	return(-1);
	}

return(fd);
}

/**********************************************************************
FUNCTION:	int _simpl_destroyFifo()

PURPOSE:	Remove receive/reply fifos.

RETURNS:	success: 0
			failuyre: -1
***********************************************************************/

int _simpl_destroyFifo(int type)
{
const char *fn = "_simpl_createFifo";
char name[128];
// WHO_AM_I _simpl_myStuff is global; populated in name_attach()
// char *_simpl_fifoPath is global; set early on in name_attach()

if (type == RECEIVE)
	{
	// name receive fifo
	sprintf(name, "%s/%s.%d",	_simpl_fifoPath,
								_simpl_myStuff.whom,
								_simpl_myStuff.pid);
	}
else if (type == REPLY)
	{
	// name reply fifo
	sprintf(name, "%s/Y%s.%d",	_simpl_fifoPath,
								_simpl_myStuff.whom,
								_simpl_myStuff.pid);
	}
else
	{
	_simpl_setErrorCode(FIFO_NAMING_FAILURE);
	_simpl_log("%s: unable to name fifo\n", fn);
	return(-1);
	}

remove(name);

return(0);
}

/**********************************************************************
FUNCTION:	int _simpl_statFifoName(const char *, int)

PURPOSE:	Check whether a corresponding fifo exists and if
			so, extract its pid component, use the pid to check if
			that process still exists and if not, remove fifos if the action
			int is TRUE, otheriwse only report the  orphaned fifos. A fifo
			is only considered valid if the parent process still exists.

RETURNS:	success (valid fifo): 0
			failure (no valid fifo): -1

NOTE:		This function will remove all invalid fifos which match
			the simpl name.
***********************************************************************/

int _simpl_statFifoName(const char *name, int action)
{
const char *fn = "_simpl_statFifoName";
DIR *directory;
struct dirent *file;
int len;
char fifoFile[128];
pid_t pid;
int ret = -1;
// char *_simpl_fifoPath is global

// open the fifo directory
directory = opendir(_simpl_fifoPath);
if (directory == NULL)
	{
	_simpl_setErrorCode(FIFO_DIRECTORY_OPEN_ERROR);
	_simpl_log("%s: cannot open %s\n", fn, _simpl_fifoPath);
	return(-1);
	}

// the length of the simpl name passed in
len = strlen(name);

// check fifo directory entries for a match
while ( (file = readdir(directory)) != NULL )
	{
	// check for a match
	if (file->d_name[len] == '.' && !memcmp(file->d_name, name, len))
		{
		// extract the pid; fifo name is of the form: name.1234
		pid = atoi(file->d_name + len + 1);

		/*
		we will make a system call on this process pid to ascertain
		its state of existence.
		getpgid() [if (getpgid(pid) == -1)] would have beeen a good
		choice but a strange warning arises during compilation so it
		has been discarded and getpriority() used in its place.
		*/

		// errno must be cleared prior to getpriority() call
		errno = 0;
		if (getpriority(PRIO_PROCESS, pid) == -1)
			{
			if (errno == ESRCH)
				{
				if (action == REMOVE)
					{
					// this program is not running so remove old fifos
					sprintf(fifoFile, "%s/%s", _simpl_fifoPath, file->d_name);
					remove(fifoFile);
					sprintf(fifoFile, "%s/Y%s", _simpl_fifoPath, file->d_name);
					remove(fifoFile);
					}
				}
			}
		else
			{
			// this program is running, return the affirmative
			ret = 0;
			}

		break;
		}
	}

closedir(directory);

return(ret);
}

/**********************************************************************
FUNCTION:	int _simpl_getFifoName(const char *, char *)

PURPOSE:	Find a receive fifo based on the simpl name.

RETURNS:	success: 0 
			failure: -1
***********************************************************************/

int _simpl_getFifoName(const char *simplName, char *fifoName)
{
const char *fn = "_simpl_getFifoName";
DIR *directory;
struct dirent *file;
int len;
int ret = -1;
// char *_simpl_fifoPath is global

// open the fifo directory
directory = opendir(_simpl_fifoPath);
if (directory == NULL)
	{
	_simpl_setErrorCode(FIFO_DIRECTORY_OPEN_ERROR);
	_simpl_log("%s: cannot open %s\n", fn, _simpl_fifoPath);
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
		sprintf(fifoName, "%s/%s", _simpl_fifoPath, file->d_name);
		ret = 0;
		break;
		}
	}

closedir(directory);

return(ret);
}

/**********************************************************************
FUNCTION:	int _simpl_FifoRename(const char *, pid_t)

PURPOSE:	rename a fifo based on the simpl name.

RETURNS:	success: 0 
			failure: -1
***********************************************************************/

int _simpl_FifoRename(const char *oldSimplName, const char *newSimplName, pid_t mypid)
{
//const char *fn = "_simpl_FifoRename";
int ret = -1;
char oldName[128];
char newName[128];
// char *_simpl_fifoPath is global

sprintf(oldName,"%s/%s.%d", _simpl_fifoPath, oldSimplName, mypid);
sprintf(newName,"%s/%s.%d", _simpl_fifoPath, newSimplName, mypid);
ret=rename(oldName, newName);
if (ret == 0)
	{
	sprintf(oldName,"%s/Y%s.%d", _simpl_fifoPath, oldSimplName, mypid);
	sprintf(newName,"%s/Y%s.%d", _simpl_fifoPath, newSimplName, mypid);
	ret=rename(oldName, newName);
	if (ret == 0)
		{
		sprintf(_simpl_myStuff.whom, "%.*s",MAX_PROGRAM_NAME_LEN,newSimplName);
		}
	}

return(ret);
}

/**********************************************************************
FUNCTION:	int _simpl_readFifoMsg(int, char *)

PURPOSE:	read any pending bytes from specified fifo fd.

RETURNS:	success: sizeof(FIFO_MSG)
			failure: != sizeof(FIFO_MSG)
**********************************************************************/	

int _simpl_readFifoMsg(int fd, char *buf)
{
int numBytes = 0;
int bytesToGo;
char *p = buf;
register int i;
int rc;
#ifdef _CYGWIN
fd_set inset;

FD_ZERO(&inset);
FD_SET(fd, &inset);
#endif

for (i = 0; i < 10; i++)
	{
	bytesToGo = sizeof(FIFO_MSG) - numBytes;
	if (bytesToGo <= 0)
		{
		break;
		}
#ifdef _CYGWIN
	select(fd+1,&inset,NULL,NULL,NULL);
#endif
	rc = read(fd, p, bytesToGo);
	numBytes += rc;
	p += rc;
	}

return(numBytes);
}

/********************************************************************/
/********************* MISCELLANEOUS FUNCTIONS **********************/
/********************************************************************/

/**********************************************************************
FUNCTION:	int saveSenderId(char *)

PURPOSE:	This function adds a reply blocked sender to the global
			array.

RETURNS:	success: 0
			failure: -1
***********************************************************************/

int saveSenderId(char *sender)
{
register int i;
// char *_simpl_blockedSenderId[] is global

// take the first available slot
for (i = 0; i < MAX_NUM_BLOCKED_SENDERS; i++)
	{
	if (_simpl_blockedSenderId[i] == (char *)NULL)
		{
		_simpl_blockedSenderId[i] = sender;
		return(0);
		}
	}

return(-1);
}

/**********************************************************************
FUNCTION:	int removeSenderId(char *)

PURPOSE:	This function removes a reply blocked sender to the global
			array.

RETURNS:	success: 0
			failure: -1
***********************************************************************/

int removeSenderId(char *sender)
{
register int i;
// char *_simpl_blockedSenderId[] is global

// initialize table of reply-blocked senders
for (i = 0; i < MAX_NUM_BLOCKED_SENDERS; i++)
	{
	if (sender == _simpl_blockedSenderId[i])
		{
		_simpl_blockedSenderId[i] = (char *)NULL;
		return(0);
		}
	}

return(-1);
}

/**********************************************************************
FUNCTION:	int ReplyError(char *)

PURPOSE:	This function replies an error condition to the sender.

RETURNS:	success: 0
			failure: -1
***********************************************************************/

int ReplyError(char *sender)
{
// const static char *fn = "ReplyError";
char fifoName[128];
int fd;
char fifoBuf[sizeof(FIFO_MSG)];
FIFO_MSG *fifoMsg = (FIFO_MSG *)fifoBuf;
FCMSG_REC *msgPtr;
// WHO_AM_I _simpl_myStuff is global 
// char *_simpl_fifoPath is global

// don't need to worry about this guy anymore
removeSenderId(sender);

msgPtr = (FCMSG_REC *)sender;

// set the fifo path and name
sprintf(fifoName, "%s/Y%s.%d",	_simpl_fifoPath,
								msgPtr->whom,
								msgPtr->pid);

// detach shmem from this process
#ifdef _EXPMTL
px_shmdt(sender);
#else
shmdt(sender);
#endif

// set up fifo message, -1 indicates an error condition
fifoMsg->shmid = -1;

// open the fifo
fd = open(fifoName, O_WRONLY);
if (fd == -1)
	{
	_simpl_setErrorCode(FIFO_OPEN_FAILURE);
	return(-1);
	}

// write the fifo trigger message
if (write(fd, fifoBuf, sizeof(FIFO_MSG)) != sizeof(FIFO_MSG))
	{
	_simpl_setErrorCode(FIFO_WRITE_FAILURE);
	close(fd);
	return(-1);
	}

// close the fifo 
close(fd);

// if we got this far the message has been sent
return(0);
}

/**********************************************************************
FUNCTION:	void _simpl_setErrorCode(int)

PURPOSE:	set the _simpl_error global variable

RETURNS:	nothing	
**********************************************************************/	

inline void _simpl_setErrorCode(int errorNumber)
{
// int _simpl_errno is global

_simpl_errno = errorNumber;
}

/**********************************************************************
FUNCTION:	void _simpl_log(char *, ...)

PURPOSE:	simpl errors/messages are recorded to file that is not
			allowed to grow past a certain limit.

RETURNS:	nothing	
**********************************************************************/	

void _simpl_log(char *format, ...)
{
va_list args;
char str[150];
int fd;
struct stat buf;
const static char *file = SIMPL_LOG_FILE;
int len;
int year;
time_t time_of_day;
struct tm *tmPtr;

// set the dateStr
time_of_day = time(NULL);
tmPtr = localtime(&time_of_day);
year = tmPtr->tm_year;
if (year > 99)
	{
	year -= 100;
	} 

// compose message string
memset(str, 0, 150);
len = sprintf(str, "%02d%02d%02d:%02d%02d%02d:%s: ",
					year,
					tmPtr->tm_mon+1,
					tmPtr->tm_mday,
					tmPtr->tm_hour,
					tmPtr->tm_min,
					tmPtr->tm_sec,
					_simpl_myStuff.whom);
va_start(args, format);
// NOTE: vnsprintf returns -1 if the text string was truncated
vsnprintf(str+len, 149-len, format, args);
va_end(args);

// get the poop on the file
if ( !lstat(file, &buf) )
	{
	if (buf.st_size < MAX_SIMPL_LOG_SIZE)
		{
		// file size is within limits
		fd = open(file, O_WRONLY | O_APPEND, 0644);
		}
	else
		{
		// file exists and is too large so truncate it
		fd = open(file, O_WRONLY | O_TRUNC | O_CREAT, 0644);
		}
	}
else
	{
	// file doesn't exist
	fd = open(file, O_WRONLY  | O_CREAT, 0644);
	}

if (fd != -1)
	{
	write(fd, str, strlen(str));
	close(fd);
	}
}

/**********************************************************************
FUNCTION:	int _simpl_check(void)

PURPOSE:	an indicator that the calling process is indeed registered
			as a SIMPL process.

RETURNS:	success: 0
			failure: -1
**********************************************************************/	

inline int _simpl_check()
{
// WHO_AM_I _simpl_myStuff is global

return(_simpl_myStuff.pid == -1 ? -1 : 0);
}

/**********************************************************************
FUNCTION:	_simpl_initSignalHandling(void)

PURPOSE:	Set up any necessary signal handling. 

RETURNS:	nothing
***********************************************************************/

void _simpl_initSignalHandling()
{
struct sigaction sigact;

// initialize the sigact structure
memset(&sigact, 0, sizeof(sigact));

// set the signal handler
sigact.sa_handler = _simpl_hndlSignals;

// the signals to trap
sigaction(SIGHUP, &sigact, NULL);
sigaction(SIGILL, &sigact, NULL);
sigaction(SIGABRT, &sigact, NULL);
sigaction(SIGINT, &sigact, NULL);
sigaction(SIGSEGV, &sigact, NULL);
sigaction(SIGTERM, &sigact, NULL);
sigaction(SIGPIPE, &sigact, NULL);
}

/**********************************************************************
FUNCTION:	void _simpl_hndlSignals(int)

PURPOSE:	Called as a response to a trapped signal.

RETURNS:	nothing
***********************************************************************/

void _simpl_hndlSignals(int signo)
{
// WHO_AM_I _simpl_myStuff is global

// log the signal
//_simpl_log("received signal %d\n", signo);

// react to signal type
switch (signo)
	{
	case SIGPIPE:
		//do nothing
		break;

	default:
		// cleanup user stuff
		if (_simpl_myStuff.myExit != NULL)
			{
			_simpl_myStuff.myExit();
			}

		// clean up SIMPL stuff
		_simpl_exitFunc();

		// don't call atexit() because we have already run _simpl_exitFunc()
		_exit(0);
	}
}

/**********************************************************************
FUNCTION:	void _simpl_exitFunc(void)

PURPOSE:	Clean up when process exits. Called by atexit() and as the
			signal handler.

RETURNS:	nothing
***********************************************************************/

void _simpl_exitFunc()
{
/*
check since the user may have remembered to name_detach() at least
in the case of a "graceful" exit
*/
if (_simpl_check() != -1)
	{
	name_detach();
	}
}

/**********************************************************************
FUNCTION:	int _simpl_postMsg(int, void *, unsigned)

PURPOSE:	This function sends simpl messages to other processes.

RETURNS:	success: 1
			failure: -1
***********************************************************************/

int _simpl_postMsg(int fd, void *outBuffer, unsigned outBytes, unsigned inBytes)
{
const static char *fn = "_simpl_postMsg";
char fifoBuf[sizeof(FIFO_MSG)];
FIFO_MSG *fifoMsg = (FIFO_MSG *)fifoBuf;
unsigned bufSize;
FCMSG_REC *msgPtr;
// WHO_AM_I _simpl_myStuff is global 
// char *_simpl_fifoPath is global

// is this program name attached?
if (_simpl_check() == -1)
	{
	_simpl_setErrorCode(NO_NAME_ATTACHED);
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
// replaced 2008Dec17
//		_simpl_deleteShmem();
		_simpl_detachShmem();
		}
	
	// create new shmem	
	if (_simpl_createShmem(bufSize) == -1)
		{
		return(-1);
		}
	}

// copy the message into shmem to be read by the receiver
msgPtr = (FCMSG_REC *)_simpl_myStuff.shmPtr;
strcpy(msgPtr->whom, _simpl_myStuff.whom);
msgPtr->pid = _simpl_myStuff.pid;
msgPtr->nbytes = outBytes;
msgPtr->ybytes = inBytes;
memcpy((void *)&msgPtr->data, outBuffer, outBytes);

#if 0
// open reply fifo if first time
if (_simpl_myStuff.y_fd == -1)
	{
	char fifoName[128];

	sprintf(fifoName, "%s/Y%s.%d",	_simpl_fifoPath,
									_simpl_myStuff.whom,
									_simpl_myStuff.pid);
	_simpl_myStuff.y_fd = open(fifoName, O_RDWR);
	if (_simpl_myStuff.y_fd == -1)
		{
		_simpl_setErrorCode(FIFO_OPEN_FAILURE);
		_simpl_log("%s: unable to open reply fifo %s-%s\n", fn, fifoName, strerror(errno));
		return(-1);
		} 
	}
#endif

// line up the triggering message for the fifo
fifoMsg->shmid = _simpl_myStuff.shmid;

// receiver reads the fifo and then sender's shmem
if (write(fd, fifoBuf, sizeof(FIFO_MSG)) != sizeof(FIFO_MSG))
	{
	_simpl_setErrorCode(FIFO_WRITE_FAILURE);
	_simpl_log("%s: unable to write to fifo-%s\n", fn, strerror(errno));
	return(-1);
	}

return(0);
} // end _simpl_postMsg

/**********************************************************************
FUNCTION:	int _simpl_readReply(void *, unsigned)

PURPOSE:	This function grabs the reply message.

RETURNS:	success: number of bytes from Reply >= 0
			failure: -1
***********************************************************************/

int _simpl_readReply(void *inBuffer, unsigned inBytes)
{
const static char *fn = "_simpl_readReply";
char fifoBuf[sizeof(FIFO_MSG)];
FIFO_MSG *fifoMsg = (FIFO_MSG *)fifoBuf;
FCMSG_REC *msgPtr;
// WHO_AM_I _simpl_myStuff is global 
// char *_simpl_fifoPath is global

msgPtr = (FCMSG_REC *)_simpl_myStuff.shmPtr;
msgPtr->ybytes = inBytes;

// wait for the receiver to send fifo message to trigger the reply
if (_simpl_readFifoMsg(_simpl_myStuff.y_fd, fifoBuf) != sizeof(FIFO_MSG))
	{
	_simpl_setErrorCode(FIFO_READ_FAILURE);
	_simpl_log("%s: unable to read from fifo\n", fn);
	close(_simpl_myStuff.y_fd);
	_simpl_myStuff.y_fd = -1;
	return(-1);
	}

// was there a problem in the send?
if (fifoMsg->shmid == -1)
	{
	_simpl_setErrorCode(COMMUNICATION_ERROR);
	_simpl_log("%s: Receive/Reply problem\n", fn);
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
} // end _simpl_readReply

#ifdef _EXPMTL
/*========================================
	px_shmget - entry point
========================================*/
int px_shmget(int key, int mysize, int shmflag)
{
char shmName[64];
int shmfd;
int rc;

rc=_simpl_myStuff.pid;
sprintf(shmName,"/shm%08X", rc);

//printf("px_shmget: shmName=%s mysize=%d\n",shmName, mysize);

shmfd = shm_open(shmName, O_CREAT | O_EXCL | O_RDWR, S_IRWXU | S_IRWXG);
if (shmfd < 0) 
	{
//printf("px_shmget: error on shm_open\n");
	return(-1);
	}

ftruncate(shmfd, mysize);

close(shmfd);

//printf("px_shmget: rc=%d\n",rc);

return(rc);

}// end px_shmget

/*==============================================
	px_shmat -entry point
===============================================*/
void *px_shmat(int shmid, const void* shmaddr, int shmflag)
{
char shmName[64];
int shmfd;
struct stat sb;
void *rc=NULL;

sprintf(shmName,"/shm%08X", shmid);
//printf("px_shmat: shmName=%s\n",shmName);

shmfd = shm_open(shmName, O_RDWR, S_IRWXU | S_IRWXG);
if (shmfd < 0) 
	{
//printf("px_shmat: got error on shm_open\n");
	return(NULL);
	}

if(fstat(shmfd, &sb) == -1)
	{
//printf("px_shmat: got error on fstat\n");
	return(NULL);
	}

rc = mmap(NULL, sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);

close(shmfd);

//printf("px_shmat: rc=0x%X\n",rc);

return(rc);

}// end px_shmat


/*==============================================
	px_shmdt -entry point
===============================================*/
int px_shmdt(void *shmaddr)
{
FCMSG_REC *shmPtr;
char shmName[64];
pid_t thisPid;
int rc=0;

//printf("px_shmdt: shmaddr=0x%X\n", shmaddr);

shmPtr=(FCMSG_REC *)shmaddr;
thisPid=shmPtr->pid;

//printf("px_shmdt: shmsize=%d\n", shmPtr->shmsize);
//printf("px_shmdt: shmPtr->pid=%d\n", shmPtr->pid);
rc = munmap(shmaddr, shmPtr->shmsize);

if(rc == 0)
	{
//printf("px_shmdt: go1\n");
//printf("px_shmdt: thisPid=%d\n", thisPid);
	if(thisPid == _simpl_myStuff.pid)
		{
		sprintf(shmName,"/shm%08X",thisPid);
//printf("px_shmdt: shmName=%s\n",shmName);

		rc=shm_unlink(shmName);
		}
	}

//printf("px_shmdt: rc=%d\n",rc);

return(rc);

}// end px_shmdt
#endif
