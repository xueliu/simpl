/*======================================================
FILE:		socketUtils.c

DESCRIPTION:	
This file contains

AUTHOR:		R.D. Findlay

-----------------------------------------------------------------------
    Copyright (C) 1999,2002,2006 FCSoftware Inc. 

    This software is in the public domain.
    Permission to use, copy, modify, and distribute this software and its
    documentation for any purpose and without fee is hereby granted, 
    without any conditions or restrictions.
    This software is provided "as is" without express or implied warranty.

    If you discover a bug or add an enhancement contact us on the 
    SIMPL project mailing list. 

-----------------------------------------------------------------------
Revision history:
=======================================================
$Log: socketUtils.c,v $
Revision 1.3  2010/01/30 11:10:46  bobfcsoft
NO_ZOMBIE default

Revision 1.2  2009/07/20 20:23:25  bobfcsoft
catch readBytesFromSocket =0 case

Revision 1.1  2009/01/19 20:15:09  bobfcsoft
relocated files

Revision 1.7  2008/03/05 17:03:44  bobfcsoft
use PF_INET in socket call

Revision 1.6  2008/03/05 16:59:25  bobfcsoft
added htons for port

Revision 1.5  2008/03/05 16:51:19  bobfcsoft
fixed memset problem

Revision 1.4  2007/07/24 21:47:20  bobfcsoft
new contact info

Revision 1.3  2007/03/07 17:52:39  bobfcsoft
trusted network change

Revision 1.2  2006/02/07 17:34:48  bobfcsoft
Tcl/Tk enhancements

Revision 1.1.1.1  2005/03/27 11:50:46  paul_c
Initial import

Revision 1.10  2003/09/11 15:42:54  root
added MSG_WAITALL and recv() in place of read() on socket

Revision 1.9  2002/11/22 16:31:35  root
2.0rc3

Revision 1.8  2002/06/12 18:27:52  root
changes for ack timer
fcipc merge completed

Revision 1.7  2002/04/16 19:09:52  root
conform to network byte order for port

Revision 1.6  2001/12/15 12:25:56  root
more error cleanup stuff

Revision 1.5  2001/12/14 22:46:32  root
pass thru write failure on relayToSocket

Revision 1.4  2001/06/18 15:56:00  root
cleanup
.l

Revision 1.3  2000/10/13 13:41:43  root
some cleanup and LGPL

Revision 1.2  2000/01/21 14:15:29  root
added runaway condition check and exit handling

Revision 1.1  1999/11/04 15:39:45  root
Initial revision

Revision 1.5  1999/08/23 11:14:12  root
added logging

Revision 1.4  1999/08/17 01:19:53  root
placed debug messages in ifdef DEBUG block

Revision 1.3  1999/08/10 19:10:09  root
cleanup

Revision 1.2  1999/08/03 22:34:57  root
more debugging

Revision 1.1  1999/07/13 18:09:32  root
Initial revision

=======================================================
=====================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <signal.h>
#ifdef ZOMBIE
#include <sys/wait.h> // for killZombies stuff
#endif

static int waitingForAck=0;  // 0 - not waiting, 1 - waiting

#define _ALLOC extern
#include "socketDefs.h"
#include "loggerVars.h"
#undef _ALLOC

#include "socketProto.h"
#include "surroMsgs.h"
#include "loggerProto.h"

/*===========================================
   	slayProcess - entry point
===========================================*/
int slayProcess(char *myname, int mypid)
{
static char *fn="slayProcess";
DIR *directory;
struct dirent *file;
int len;
char *fifoPath;
pid_t pid;
int rc=0;

// get the fifo directory
fifoPath = getenv("FIFO_PATH");
if (fifoPath == NULL)
	{
	fcLogx(__FILE__, fn,
		TCL_SURROGATE_MISC,
		TCL_SURROGATE_MISC,
	"Unable to obtain fifo path-%s", 
	strerror(errno));

	return(-1);
	}

// the length of the simpl name passed in
len = strlen(myname);

// open the fifo directory
directory = opendir(fifoPath);
if (directory == NULL)
	{
	fcLogx(__FILE__, fn,
		TCL_SURROGATE_MISC,
		TCL_SURROGATE_MISC,
	"Cannot open fifo directory-%s", strerror(errno));

	return(-1);
	}

// check all fifo directory entries
while ( (file = readdir(directory)) != NULL )
	{
	// check for a match
	if (!memcmp(file->d_name, myname, len) && file->d_name[len] == '.')
		{
		// extract the pid from the fifo name
		pid = atoi(&file->d_name[len + 1]);

		if (mypid == pid)
			{	
		// kill this process
			rc=kill(pid, SIGTERM);
			if(rc == -1)
				{
			char name[128];

// remove Receive fifo
			sprintf(name, "%s/%s",		
				fifoPath,
				file->d_name);
			remove(name);

// remove Reply fifo
			sprintf(name, "%s/Y%s",
				fifoPath,
				file->d_name);
			remove(name);
				}
			} // end if mypid
		}
	}

closedir(directory);

fcLogx(__FILE__, fn,
	globalMask,
	TCL_SURROGATE_FUNC_IO,
	"rc=%d",
	rc
	);

return(rc);
}/* end slayProcess */

/*===========================================
   	getMyHost - entry point
===========================================*/
char *getMyHost()
{
#if 0
static char *fn="getMyHost";
#endif
static char myHost[256];

gethostname(myHost,256);
return(myHost);
}/* end getMyHost */

/*===========================================
   	setWaitingForAck - entry point
===========================================*/
int setWaitingForAck()
{
static char *fn="setWaitingForAck";

fcLogx(__FILE__, fn,
	globalMask,
	TCL_SURROGATE_FUNC_IO,
	"ding"
	);

waitingForAck=1;

return(waitingForAck);
}/* end setWaitingForAck */

/*===========================================
   	clrWaitingForAck - entry point
===========================================*/
int clrWaitingForAck()
{
static char *fn="clrWaitingForAck";

fcLogx(__FILE__, fn,
	globalMask,
	TCL_SURROGATE_FUNC_IO,
	"ding"
	);

waitingForAck=0;

return(waitingForAck);
}/* end clrWaitingForAck */

/*===========================================
   	isWaitingForAck - entry point
===========================================*/
int isWaitingForAck()
{
static char *fn="isWaitingForAck";

fcLogx(__FILE__, fn,
	globalMask,
	TCL_SURROGATE_FUNC_IO,
	"waitingForAck=%d",
	waitingForAck
	);

return(waitingForAck);
}/* end isWaitingForAck */

/*===========================================
	detachSocket - entry point
===========================================*/
void detachSocket(int mySocket)
{
#if 0
static char *fn="detachSocket";
#endif

shutdown(mySocket, 2);

} /* end detachSocket */

/*===========================================
	attachSocket - entry point
===========================================*/
int attachSocket(int ihostport)
{
static char *fn="attachSocket";
int                  s;            /* "well known" socket descriptor    */
int                  rc;           /* return code                       */
int                  attempts = 5; /* Max attempts client is allowed    */
struct sockaddr_in   sin;          /* socket parameters: address, port  */
int 		     offset;
int 		     hostport;

errno = 0;                         /* UNIX global error code            */

offset = 0;
hostport = ihostport + offset;


/***************************************************************************/
/*                                                                         */
/* Create an INTERNET socket                                               */
/*                                                                         */
/***************************************************************************/
s = socket(PF_INET, SOCK_STREAM, 0);
if (s == -1)
   {
   printf("File:%s line:%d process:%s %s %d - error %d\n"
               ,__FILE__
               ,__LINE__
               , fn 
               ,"unable to create socket"
	       , hostport	 	
               , errno
               );
    return (-1);
    }

#ifdef DEBUG
printf("%s:hostport=%d\n",fn,hostport);
#endif

/***************************************************************************/
/*                                                                         */
/* Set the socket fields: server listens on hostport                       */
/*                                                                         */
/***************************************************************************/
memset(&sin, 0, sizeof(sin));
sin.sin_family      = AF_INET;
sin.sin_addr.s_addr = INADDR_ANY;
sin.sin_port        = htons(hostport);

/***************************************************************************/
/*                                                                         */
/* Bind the address/port to the socket                                     */
/*                                                                         */
/***************************************************************************/
rc = bind(s, (struct sockaddr *)&sin, sizeof(sin));
if (rc == -1)
     {
         printf("File:%s line:%d process:%s %s %d - error %d: %s\n"
               ,__FILE__
               ,__LINE__
               , fn 
               ,"unable to bind address to socket"
               , hostport
               , errno
               ,strerror(errno)
               );
         return (-1);
     }


{
int length;

length = sizeof(sin);
if(getsockname(s, (struct sockaddr *)&sin, &length))
	{
	printf("problems\n");
	exit(1);
	}

#ifdef DEBUG
printf("Socket has port %d[%d]\n",ntohs(sin.sin_port),sin.sin_port);
#endif
}



/***************************************************************************/
/*                                                                         */
/* Listen for clients attempts to connect.  We limit clients to 5          */
/*                                                                         */
/***************************************************************************/
if (listen(s, attempts) == -1)
   {
   printf("File:%s line:%d process:%s %s\n"
               ,__FILE__
               ,__LINE__
               , fn
               ,"Client processes exceed 5 in queue"
               );
         return (1);
    }

return(s);

}/* end attachSocket*/

/*===========================================
	acceptSocket - entry point
===========================================*/
int acceptSocket(int s)
{
static char *fn="acceptSocket";
int ns;

/***********************************************************************/
/*                                                                     */
/* Accept the client connect() function.  Like answering a telephone   */
/*                                                                     */
/***********************************************************************/
ns = accept(s, 0, 0);
if (ns == -1)
   {
   printf("File:%s line:%d process:%s %s\n"
                   ,__FILE__
                   ,__LINE__
                   , fn
                   ,"cannot create new (service) socket for client"
                   );
    }

return(ns);
} // end acceptSocket

/*===========================================
	connectSocket - entry point
===========================================*/
int connectSocket(char *hostname, int ihostport) 
{
static char *fn="connectSocket";
struct hostent      *hp;
struct sockaddr_in   sin;
int 			s;
int 		      hostport;
int                   offset;

offset = 0;
hostport = htons(ihostport + offset);

/***************************************************************************/
/*                                                                         */
/* Look in standard UNIX file /etc/hosts to look up host name              */
/*                                                                         */
/***************************************************************************/
hp = gethostbyname(hostname);
if (hp == NULL)
   {
   printf("File:%s line:%d process:%s gethostbyname failed for %s"
           ,__FILE__
           ,__LINE__
           , fn
	   , hostname
           );

    return (1);
    }

/***************************************************************************/
/*                                                                         */
/* Set the socket fields: port 8888 on host specified on command line parm */
/*                                                                         */
/***************************************************************************/
memset(&sin, sizeof(sin), 0);
sin.sin_family = hp->h_addrtype;
sin.sin_port = hostport;                          /* Set SERVER port    */
memcpy(&sin.sin_addr, hp->h_addr, hp->h_length);  /* Set SERVER address */

/***************************************************************************/
/*                                                                         */
/* Create an INTERNET socket                                               */
/*                                                                         */
/***************************************************************************/
s = socket(AF_INET, SOCK_STREAM, 0);
if (s == -1)
   {
   printf("File:%s line:%d process:%s %s\n"
               ,__FILE__
               ,__LINE__
               , fn
               ,"unable to create socket"
               );
   return (1);
   }


/***************************************************************************/
/*                                                                         */
/* Connect (or MAKE CALL) to SERVER's accept() function                    */
/* Structure 'sin' contains address of SERVER                              */
/*                                                                         */
/***************************************************************************/
if (connect(s, (struct sockaddr *)&sin, sizeof(struct sockaddr_in)) == -1)
   {
   printf("File:%s line:%d process:%s %s port:%d\n"
               ,__FILE__
               ,__LINE__
               , fn
               ,"failed to connect to server"
               , sin.sin_port
               );
   return (1);
   }
return(s);
} // end connectSocket

/*===========================================
	relayToSocket - entry point
	non blocking send
===========================================*/
int relayToSocket( int s,
	char *outbuf, 
	int outlen)
{
static char *fn="relayToSocket";
int rc;

fcLogx(__FILE__, fn,
	globalMask,
	TCL_SURROGATE_MISC,
	"socket=%d nbytes=%d",
	s,
	outlen
	);

#ifdef DEBUG
fcLogx(__FILE__, fn,
	globalMask,
	TCL_SURROGATE_MISC,
	"%X-%X-%X-%X",
	outbuf[0],
	outbuf[1],
	outbuf[2],
	outbuf[3]);
#endif

rc = write(s, outbuf, outlen );
if (rc == -1)
	{
	fcLogx(__FILE__, fn,
		TCL_SURROGATE_MARK,
		TCL_SURROGATE_MARK,
		"failed to send %d bytes on socket=%d",
		outlen,
		s
		);
   	}

return(rc);

} /* end relayToSocket */

/*===========================================
	sendToSocket - entry point
===========================================*/
int sendToSocket( int s,
	char *outbuf, 
	int outlen,
	char *inbuf,
	int inlen)
{
static char *fn="sendToSocket";
int rc;
SG_REPLY_MSG *incoming;
char *p;
int tbytes;

incoming=(SG_REPLY_MSG *)inArea;
p=inArea;

#ifdef DEBUG
fcLogx(__FILE__, fn,
	globalMask,
	TCL_SURROGATE_MISC,
	"%X-%X-%X-%X",
	outbuf[0],
	outbuf[1],
	outbuf[2],
	outbuf[3]);
#endif

rc = write(s, outbuf, outlen );
if (rc == -1)
   {
   printf("File:%s line:%d process:%s %s\n"
               ,__FILE__
               ,__LINE__
               , fn
               ,"failed to send bytes to server\n"
               );
   return (1);
   }

rc = read(s, inArea, sizeof(int));
p += rc;
tbytes=rc-sizeof(int);

if(incoming->token == SG_REPLY_IT)
	{
#ifdef DEBUG
fcLogx(__FILE__, fn,
	globalMask,
	TCL_SURROGATE_MISC,
	"reply expecting %d bytes ... got %d bytes so far",
	incoming->nbytes, 
	tbytes);
#endif
	}
else
	printf("%s: unknown token=%d rec'd in reply\n",fn,incoming->token);

rc = read(s, p, incoming->nbytes+4-rc );    // read the rest
tbytes += rc;

#ifdef DEBUG
fcLogx(__FILE__, fn,
	globalMask,
	TCL_SURROGATE_MISC,
	"reply expecting %d bytes ... got %d bytes so far",
	incoming->nbytes, 
	tbytes);
#endif

if (rc == -1)
   {
   printf("File:%s line:%d process:%s %s\n"
            ,__FILE__
               ,__LINE__
               , fn
               ,"failed to read bytes from server\n"
               );
   return (1);
   }
return(rc);

} /* end sendToSocket */

/*==============================================
	receiveFromSocket - entry point
==============================================*/
int receiveFromSocket(int ns, char *inbuf)
{
static char *fn="receiveFromSocket";
int rbytes;
int numBytes;
char *p;
SG_SEND_MSG *incoming;
int rc;

fcLogx(__FILE__, fn,
	globalMask,
	TCL_SURROGATE_MISC,
	"ding"
	);

rc=ns;
p=inbuf;
incoming = (SG_SEND_MSG *)inbuf;

// on first pass read just header
numBytes=sizeof(int);
rbytes = readBytesFromSocket(ns, numBytes, p);
if(rbytes == -1 )   
	{
	fcLogx(__FILE__, fn,
		TCL_SURROGATE_MARK,
		TCL_SURROGATE_MARK,
		"failed to read %d bytes from socket=%d rbytes=%d",
		numBytes,
		ns,
		rbytes
		);

   	return (-1);
   	}

p +=rbytes;

// on second pass read the balance
numBytes=incoming->nbytes;

#ifdef DEBUG
fcLogx(__FILE__, fn,
	globalMask,
	TCL_SURROGATE_MISC,
	"got %d bytes (%X-%X-%X-%X) ... expecting %d more bytes",
	rbytes,
	inbuf[0],
	inbuf[1],
	inbuf[2],
	inbuf[3],
	numBytes);
#endif

rbytes = readBytesFromSocket(ns, numBytes, p);
if(rbytes == -1 )   
	{
	fcLogx(__FILE__, fn,
		TCL_SURROGATE_MARK,
		TCL_SURROGATE_MARK,
		"failed to read %d bytes from socket=%d rbytes=%d",
		numBytes,
		ns,
		rbytes
		);

   	return (-1);
   	}

#ifdef DEBUG
fcLogx(__FILE__, fn,
	globalMask,
	TCL_SURROGATE_MISC,
	"got %d bytes (%X-%X-%X-%X-%X-%X-%X-%X)",
	rbytes,
	p[0],
	p[1],
	p[2],
	p[3],
	p[4],
	p[5],
	p[6],
	p[7]);
#endif

p +=rbytes;
p[0]=0; // null terminate
	
return(rc);

} /* end receiveFromSocket */

/*==============================================
	readBytesFromSocket - entry point
==============================================*/
int readBytesFromSocket(int ns, int rbytes, char *inbuf)
{
static char *fn="readBytesFromSocket";
int rc=-1;

fcLogx(__FILE__, fn,
	globalMask,
	TCL_SURROGATE_TRACK,
	"waiting for %d bytes from socket=%d",
	rbytes,
	ns
	);

if(rbytes == 0) return(0);

#ifdef MSG_WAITALL
rc = recv(ns, inbuf, rbytes, MSG_WAITALL);
#else
rc = recvWAITALL(ns, inbuf, rbytes);
#endif
if(rc < rbytes )   
	{
	fcLogx(__FILE__, fn,
		TCL_SURROGATE_MARK,
		TCL_SURROGATE_MARK,
		"failed to read %d bytes from socket=%d rbytes=%d",
		rbytes,
		ns,
		rc
		);

   	return (-1);
   	}

fcLogx(__FILE__, fn,
	globalMask,
	TCL_SURROGATE_TRACK,
	"got %d bytes from socket=%d",
	rc,
	ns
	);


return(rc);

} /* end readBytesFromSocket */

#ifndef MSG_WAITALL
// some older distributions don't support WAITALL flag
/*==============================================
	recvBytes - entry point
==============================================*/
int recvWAITALL(int ns, char *inbuf, int rbytes)
{
static char *fn="recvWAITALL";
int bytesRead;
int bytesToGo;
char *p;
int rc=-1;

bytesToGo=rbytes;
p=inbuf;
do
	{
	bytesRead = recv(ns, p, bytesToGo, 0);
	if(bytesRead > 0)
		{
		bytesToGo -= bytesRead;
		p +=bytesRead;
		if(bytesToGo <= 0)
			{
			rc=p-inbuf;
			break;
			}
		}
	}
	while(bytesRead > 0);

fcLogx(__FILE__, fn,
	globalMask,
	TCL_SURROGATE_MARK,
	"read %d of %d bytes from socket=%d",
	rc,
	rbytes,
	ns
	);

return(rc);

} // end recvWAITALL
#endif

/*===========================================
	replyToSocket - entry point
===========================================*/
int replyToSocket(int s, char *outbuf, int outlen)
{
static char *fn="replyToSocket";
int rc;
SG_REPLY_MSG *outMsg;

outMsg = (SG_REPLY_MSG *)outArea;

outMsg->token = SG_REPLY_IT;
outMsg->toWhom = -1;
memcpy(&outMsg->dataMark,outbuf,outlen);
outMsg->nbytes = outlen+sizeof(int);

#ifdef DEBUG
{
char *p;
p=&outMsg->dataMark;

fcLogx(__FILE__, fn,
	globalMask,
	TCL_SURROGATE_MISC,
	"reply %d bytes %X-%X-%X-%X",
	outMsg->nbytes,
	p[0],
	p[1],
	p[2],
	p[3]);

fcLogx(__FILE__, fn,
	globalMask,
	TCL_SURROGATE_MISC,
	"%d bytes: %X-%X-%X-%X-%X-%X-%X-%X",
	outMsg->nbytes,
	outArea[0],
	outArea[1],
	outArea[2],
	outArea[3],
	outArea[4],
	outArea[5],
	outArea[6],
	outArea[7]);
}
#endif

rc = write(s, outArea, outMsg->nbytes+sizeof(int) );
if (rc == -1)
   {
   printf("File:%s line:%d process:%s %s\n"
               ,__FILE__
               ,__LINE__
               , fn
               ,"failed to send bytes to server\n"
               );
   }
return (rc);
} /* end replyToSocket */

#ifdef ZOMBIE
/**********************************************************************
FUNCTION:	killZombies(void)

PURPOSE:	release zombie children by retrieving their exit status.

RETURNS:	void
**********************************************************************/	

void killZombies()
{
int i;
int status;
pid_t whodied;

for (i=1; i <= 10; i++)
	{
	whodied = waitpid(-1, &status, WNOHANG);
	if (whodied <= 0)
		{
		break;
		}
	}
} // end killZombies
#endif
