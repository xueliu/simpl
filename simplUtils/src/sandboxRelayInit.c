/*************************************************************

FILE:		sandboxRelayInit.c

DESCRIPTION:	
This file contains initialization code for sandboxRelay

AUTHOR:			R.D. Findlay

-----------------------------------------------------------------------
    Copyright (C) 2007 SIMPL project. 

    This software is in the public domain.
    Permission to use, copy, modify, and distribute this software and its
    documentation for any purpose and without fee is hereby granted, 
    without any conditions or restrictions.
    This software is provided "as is" without express or implied warranty.

    If you discover a bug or add an enhancement contact the SIMPL
    project mailing list. 

-----------------------------------------------------------------------
Revision history:
=======================================================
$Log: sandboxRelayInit.c,v $
Revision 1.2  2011/09/08 16:19:40  bobfcsoft
more portable arg processing

Revision 1.1  2007/07/03 18:28:04  bobfcsoft
added sandboxRelay

=======================================================

*************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define _ALLOC extern
#include "sandboxRelay.h"
#undef _ALLOC

#include "sandboxRelayProto.h"
#include "simplProto.h"
#include "loggerProto.h"
#include "simplmiscProto.h"

/*============================================
	initialize - entry point
============================================*/
void initialize(int argc, char **argv)
{
static char *fn="initialize";
int i;                          /* loop variable */
char myName[20];
char loggerName[20];
int myslot;

globalMask=0xff;
myName[0]=0;
recvID=-1;
sb_fd=-1;

/*===============================================
  process command args
===============================================*/
for(i=1; i<=argc; ++i)
        {
        char *p = argv[i];

        if(p == NULL) continue;

        if(*p == '-')
                {
                switch(*++p)
                        {
                        case 'n':
                                for(;*p != 0; p++);
//				sprintf(myName,"%s",++p);
				sprintf(myName,"%s",argv[++i]);
				myslot=name_attach(myName, sb_name_detach);
				if(myslot == -1)
					{
					printf("%s: unable to attach as <%s>\n",
						fn,
						myName);
					exit(0);
					}
				else
					{
					printf("attached as <%s> myslot=%d\n",myName,myslot);
					}
					
                                break;

			case 'f':
                                for(;*p != 0; p++);
//				sprintf(otherFifoPath,"%.127s",++p);
				sprintf(otherFifoPath,"%.127s",argv[++i]);
				break;

			case 'l':
                                for(;*p != 0; p++);
//				sprintf(loggerName,"%.19s",++p);
				sprintf(loggerName,"%.19s",argv[++i]);
				break;

			case 'm':
				if(*++p == 0) p++;
				p=argv[++i];
				globalMask=atoh(&p[2]);  // skip 0x
				break;

                        default:
				printf("%s:unknown arg %s\n",fn, p);
                                break;
                        }/*end switch*/
                } /* end if *p */
        }/*end for i*/

// check for compulsory args
if(myName[0] == 0)
	{
	myUsage();
	exit(0);
	}

// try to connect to trace logger
logger_ID = is_logger_upx(loggerName);

// try to connect to same name in other sandbox
recvID=sb_name_locate(myName);

if(recvID != -1)
	sb_name_attach(myName);

fcLogx(__FILE__, fn,
	0xff,
	TRACE_MARK,
	"myName=<%s> myslot=%d",
	myName,
	myslot
	);

fcLogx(__FILE__, fn,
	0xff,
	TRACE_MARK,
	"otherFifoPath=<%s>",
	otherFifoPath	
	);

fcLogx(__FILE__, fn,
	0xff,
	TRACE_MARK,
	"recvName=<%s> recvID=%d",
	myName,
	recvID	
	);

fcLogx(__FILE__, fn,
	0xff,
	TRACE_MARK,
	"trace logger mask = 0x%04X",
	globalMask);

} /* end initialize */

/*===================================================
	myUsage - entry point
===================================================*/
void myUsage()
{
printf("====================== usage =============================\n");
printf(" usage for relay:\n");
printf("      relay -n <myName> <optionals>\n");
printf("      where optionals are:\n");
printf("        -m 0x<mask> - trace logger mask override\n");
printf("        -l <loggerName> - connect to trace logger\n");
printf("==========================================================\n");
}// end myUsage
