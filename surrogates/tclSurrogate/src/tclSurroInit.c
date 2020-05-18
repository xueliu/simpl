/*************************************************************
	FILE:		tclSurroInit.c

	DESCRIPTION:	
	This file contains the surrogate initialization for Tcl/Tk
	surrogate process.

	AUTHOR:			R.D. Findlay

-----------------------------------------------------------------------
    Copyright (C) 1999, 2002, 2007 FCSoftware Inc. 

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
    $Log: tclSurroInit.c,v $
    Revision 1.4  2011/09/08 16:59:59  bobfcsoft
    more portable arg processing

    Revision 1.3  2011/03/23 17:57:17  bobfcsoft
    removed termios.h include

    Revision 1.2  2010/01/30 11:10:46  bobfcsoft
    NO_ZOMBIE default

    Revision 1.1  2009/01/19 20:15:09  bobfcsoft
    relocated files

    Revision 1.5  2008/03/05 18:42:40  bobfcsoft
    remove htons

    Revision 1.4  2007/07/24 21:47:20  bobfcsoft
    new contact info

    Revision 1.3  2007/03/07 17:54:24  bobfcsoft
    trusted network change added usage

    Revision 1.2  2007/03/07 17:52:39  bobfcsoft
    trusted network change

    Revision 1.1.1.1  2005/03/27 11:50:48  paul_c
    Initial import

    Revision 1.11  2002/11/22 16:32:20  root
    2.0rc3

    Revision 1.10  2002/11/19 15:08:39  root
    2.0rc1

    Revision 1.9  2002/07/06 12:20:12  root
    fixed bug to allow tclSurrogate to run without a logger

    Revision 1.8  2002/06/12 18:29:11  root
    add override for maxAckTime
    fcipc merge completed

    Revision 1.7  2001/12/15 12:27:18  root
    initialize the blocked senders array

    Revision 1.6  2001/10/25 00:33:24  root
    allowed port override on command line

    Revision 1.5  2001/01/27 14:20:19  root
    replaced fcqnxProto.h with simplmiscProto.h

    Revision 1.4  2000/10/13 13:43:06  root
    LGPL
    expanded to 2k message size

    Revision 1.3  1999/12/30 12:10:10  root
    childsName now a global variable

    Revision 1.2  1999/12/15 02:58:27  root
    sys/time.h replaced by time.h

    Revision 1.1  1999/11/04 15:40:19  root
    Initial revision

=======================================================

*************************************************************/
#include <stdio.h>
#include <stdlib.h> 			/* for exit etc */
#include <string.h>  			/* for memcpy etc */
#include <unistd.h>			/* for getnid */
#include <time.h>			// prototype for select
#include <sys/types.h>
#include <fcntl.h>
//#include <termios.h>
#include <netinet/in.h>                 // for htons stuff
#include <signal.h>			// for sigaction stuff

#include "simpl.h"
#include "surroMsgs.h"

#define _ALLOC extern
#include "tclSurrogate.h"
#undef _ALLOC

// Prototypes
#include "tclSurroProto.h"
#include "simplProto.h"
#include "loggerProto.h"
#include "socketProto.h"
#include "simplmiscProto.h"   // for atoh etc.

/*=========================================================
	initialize
=========================================================*/
void initialize(int argc, char **argv)
{
static char *fn="initialize";
int i;
int myslot;
unsigned short myport=8000;

// set defaults
globalMask=0xffffffff;
parentsName[0]=0;
logger_name[0]=0;
logger_ID=-1;
maxAckTime=10;  // defaults to 10 sec
trustedNetworkFlag=0; // defaults to untrusted

/*=========================================================
	process command line arguments
=========================================================*/
for (i=1; i<=argc; ++i) 
	{	
	char *p = argv[i];

	if (p == NULL) continue;

	if (*p == '-')
		{             
		switch (*++p)
			{
			case 'n':
//				if(*++p == 0) p++;
//				sprintf(parentsName,"%.19s",p);
				sprintf(parentsName,"%.19s",argv[++i]);

				myslot=name_attach(parentsName, NULL);
				if (myslot == -1)
					{
					printf("%s: Can't attach as %s\n",fn,parentsName);
					exit(0);
					}
				break;

			case 'l':
//				if(*++p == 0) p++;
//				sprintf(logger_name,"%.19s",p);
				sprintf(logger_name,"%.19s",argv[++i]);
				break;

			case 'p':
//				if(*++p == 0) p++;
//				myport=atoi(p); 
				myport=atoi(argv[++i]); 
				break;

			case 'a':
//				if(*++p == 0) p++;
//				maxAckTime=atoi(p); 
				maxAckTime=atoi(argv[++i]); 
				break;

			case 'm':
//				if(*++p == 0) p++;
				p=argv[++i];
				globalMask=atoh(&p[2]);  // skip 0x
				break;

			case 'T':
				if(*++p == 0) p++;
				trustedNetworkFlag=1;  // set to trusted
				break;

			default:
				printf("Bad command line switch `%c'\n", *p);
			} // end switch
		} // end if p
	} // end for i	

// if manditory name is not supplied show usage
if(parentsName[0] == 0)
	{
	myUsage();
	exit(0);
	}

// attach the socket
mySocket = attachSocket(myport);

// arm the select routine
my_fds[0] = mySocket;   // socket
my_fds[1] = whatsMyRecvfd();  // fifo
FD_ZERO(&watchset);
FD_SET(my_fds[0], &watchset);
FD_SET(my_fds[1], &watchset);

maxfd=0;
for(i=0; i<2; i++)
	if(maxfd < my_fds[i]) maxfd=my_fds[i];

// initialize the blocked senders array
for(i=0; i<32; i++)
	myblockedSenders[i] = NULL;

#ifdef NO_ZOMBIE
// setup to not have zombie processes
{
struct sigaction zom;

zom.sa_handler = SIG_IGN;
sigemptyset(&zom.sa_mask);
zom.sa_flags = SA_NOCLDWAIT;
sigaction(SIGCHLD, &zom, NULL);
}
#endif

// connect to logger
logger_ID = is_logger_upx(logger_name);

fcLogx(__FILE__, fn,
	0xff,
	TCL_SURROGATE_MARK,
	"myName=<%s> myslot=%d",
	parentsName,
	myslot);

fcLogx(__FILE__, fn,
	0xff,
	TCL_SURROGATE_MARK,
	"myport=%d",
	myport);

fcLogx(__FILE__, fn,
	0xff,
	TCL_SURROGATE_MARK,
	"Network flag: %s",
	(trustedNetworkFlag==1) ? "trusted" : "untrusted");

fcLogx(__FILE__, fn,
	0xff,
	TCL_SURROGATE_MARK,
	"my_fds[0]=%d my_fds[1]=%d",
	my_fds[0],
	my_fds[1]);

fcLogx(__FILE__, fn,
	0xff,
	TCL_SURROGATE_MARK,
	"logger_ID=%d",
	logger_ID);

}// end initialize

/*=========================================================
	initChild
=========================================================*/
void initChild(int counter)
{
static char *fn="initChild";
int myslot;

myIndex = counter;

child_detach();
if(logger_ID != -1)
	{
#if 0
printf("%s:closing logger_fd=%d\n",fn,logger_ID);
#endif
	close(logger_ID);
	logger_ID=-1;
	}

sprintf(childsName,"%s_%03d",parentsName,counter);
myslot=name_attach(childsName, NULL);
childsSlot=myslot;
if (myslot == -1)
	{
	printf("%s: Can't attach as %s\n",fn,childsName);
	exit(0);
	}

parents_id = name_locate(parentsName);
if(parents_id == -1)
	{
	printf("%s: Can't locate <%s>\n",fn,parentsName);
	exit(0);
	}

// connect to logger
logger_ID = is_logger_upx(logger_name);

fcLogx(__FILE__, fn,
	0xff,
	TCL_SURROGATE_MARK,
	"myName=<%s> myslot=%d recv_fd=%d",
	childsName,
	myslot,
	whatsMyRecvfd());

fcLogx(__FILE__, fn,
	0xff,
	TCL_SURROGATE_MARK,
	"parentsName=<%s> parents fd=%d",
	parentsName,
	parents_id);

fcLogx(__FILE__, fn,
	0xff,
	TCL_SURROGATE_MARK,
	"logger=<%s> logger fd=%d",
	logger_name,
	logger_ID);

fcLogx(__FILE__, fn,
	0xff,
	TCL_SURROGATE_MARK,
	"maxAckTime=%d sec",
	maxAckTime);

}// end initChild

/*===================================================
	myUsage - entry point
===================================================*/
void myUsage()
{
printf("====================== usage =====================\n");
printf(" usage for tclSurrogate:\n");
printf("      tclSurrogate -n <myName> <optionals>\n");
printf("      where optionals are:\n");
printf("        -a <maxAckTime> - max ack lag time in sec\n");
printf("        -p <portNo> - port number\n");
printf("        -m <mask> - override default log mask\n");
printf("        -l <loggerName> - connect to trace logger\n");
printf("        -T - connected to a trusted network\n");
printf("==================================================\n");
}// end myUsage

/*==================================================
	myExit - entry point
==================================================*/
int myExit()
{
#if 0
static char *fn="myExit";
#endif

detachSocket(mySocket);

#if 0
printf("%s:goodbye\n",fn);
#endif

return(1);

} // end myExit
