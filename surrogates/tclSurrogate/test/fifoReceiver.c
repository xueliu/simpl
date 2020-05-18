/*======================================
	fifoReceiver.c

Description:
This module is used to receive a message from the GUI
via the surrogate.

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
======================================*/
/*
 *$Log: fifoReceiver.c,v $
 *Revision 1.1  2009/01/19 20:16:02  bobfcsoft
 *relocated files
 *
 *Revision 1.2  2007/07/24 21:47:20  bobfcsoft
 *new contact info
 *
 *Revision 1.1.1.1  2005/03/27 11:50:49  paul_c
 *Initial import
 *
 *Revision 1.6  2002/11/22 16:33:55  root
 *2.0rc3
 *
 *Revision 1.5  2002/11/19 15:21:05  root
 *2.0rc1
 *
 *Revision 1.4  2002/06/12 18:32:54  root
 *fcipc merge completed
 *
 *Revision 1.3  2000/10/13 14:06:14  root
 *LGPL
 *
 *Revision 1.2  2000/02/15 18:57:44  root
 *fixed int main warning for 2.2 systems
 *
 *Revision 1.1  1999/12/30 12:14:31  root
 *Initial revision
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#if 0
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/timeb.h>
#endif

#include "surroTestMsgs.h"

#define _ALLOC
#include "fifoReceiver.h"
#include "loggerVars.h"
#undef _ALLOC

#include "simplProto.h"
#include "loggerProto.h"
void initialize(int argc, char **argv);
void myUsage();

/*--------------------------------------
	fifoReceiver - entry point
--------------------------------------*/
int main(int argc, char **argv, char **envp)
{
static char *fn="fifoReceiver";
UINT16 *token;
char * fromWhom;
int nbytes;

printf("%s:starting\n",fn);

initialize(argc, argv);

token=(UINT16 *)inArea;
while(1)
	{
#if 0
	fromWhom = receiveMsg(&msg, &nbytes);
	memcpy(inArea,msg,nbytes);
#endif
	nbytes=Receive(&fromWhom, inArea, 8192);

	switch(*token)
		{
		case SURRO_TEST:
			{
			SURRO_TEST_MSG *inMsg;
			SURRO_TEST_MSG *outMsg;
			static int count;
			
			inMsg=(SURRO_TEST_MSG *)inArea;
			fcLogx(__FILE__, fn,
				globalMask,
				FIFO_RECV_MISC,
				"SURRO_TEST msg=<%s>",
				inMsg->msg);

			outMsg=(SURRO_TEST_MSG *)outArea;
			outMsg->token=SURRO_TEST;
			sprintf(outMsg->msg,"%s_%d",myMsg,count++);

#if 0
			loadReply(fromWhom,outArea,sizeof(SURRO_TEST_MSG));
			replyMsg(fromWhom);
#endif
			Reply(fromWhom, outArea, sizeof(SURRO_TEST_MSG));
			}
			break;

		default:
			fcLogx(__FILE__, fn,
				globalMask,
				FIFO_RECV_MISC,
				"unknown token=%d",
				*token);

#if 0
			loadReply(fromWhom,NULL,0);
			replyMsg(fromWhom);
#endif
			Reply(fromWhom, NULL, 0);
			break;
		}
	} // end while

#if 0
outMsg = (SURRO_TEST_MSG *)outArea;
outMsg->token = SURRO_TEST;
sprintf(outMsg->msg,"%.79s",myMsg);

printf("%s:sending <%s> to %s\n",
	fn,
	myMsg,
	toName);

fcLogx(__FILE__, fn,
	globalMask,
	FIFO_SENDER_MISC,
	"sending <%s> to <%s>",
	myMsg,
	toName);

loadSend(outArea,sizeof(SURRO_TEST_MSG));
sendMsg(toPid);
n=retrieveReply(inArea);

inMsg=(SURRO_TEST_MSG *)inArea;

if(n > 0)
	{
	printf("%s:got ACK back reply=<%s>\n",fn,inMsg->msg);
	fcLogx(__FILE__, fn,
		globalMask,
		FIFO_SENDER_MISC,
		"got ACK back reply=<%s>",
		inMsg->msg);
	}
else
	{
	printf("%s:got ACK back\n",fn);
	fcLogx(__FILE__, fn,
		globalMask,
		FIFO_SENDER_MISC,
		"got ACK back");
	}
#endif

printf("%s:done\n",fn);
name_detach();

return(1);
}// end fifoReceiver

/*============================================
	initialize - entry point
============================================*/
void initialize(int argc, char **argv)
{
static char *fn="initialize";
int i;                          /* loop variable */
char myName[20];
char loggerName[20];
int rc;

globalMask=0xff;
myName[0]=0;
loggerName[0]=0;
myMsg[0]=0;

sprintf(myMsg,"hello there");

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
				sprintf(myName,"%s",++p);
				rc=name_attach(myName, NULL);
				if(rc == -1)
					{
					printf("%s: unable to attach as <%s>\n",
						fn,
						myName);
					exit(0);
					}
				else
					printf("attached as <%s> rc=%d\n",myName,rc);
                                break;

#if 0
			case 'r':
                                for(;*p != 0; p++);
				sprintf(toName,"%s",++p);
				who=nameLocatex(toName);
				if(who.slot == -1)
					{
					printf("%s: unable to locate <%s>\n",
						fn,
						toName);
					detachName();
					exit(0);
					}
				else
					{
					toPid = who.fd;
					printf("located <%s> pid=%d\n",toName,toPid);
					}
				break;
#endif

			case 'm':
                                for(;*p != 0; p++);
				sprintf(myMsg,"%s",++p);
				break;

			case 'l':
                                for(;*p != 0; p++);
				sprintf(loggerName,"%.19s",++p);
				break;

                        default:
				printf("%s:unknown arg %s\n",fn, p);
                                break;
                        }/*end switch*/
                } /* end if *p */
        }/*end for i*/

// if manditory name is not supplied show usage
if(myName[0] == 0)
	{
	myUsage();
	exit(0);
	}

#if 0
if(toName[0] == 0 || myMsg[0] == 0)
	{
	myUsage();
	detachName();
	exit(0);
	}
#endif

logger_ID = is_logger_upx(loggerName);

fcLogx(__FILE__, fn,
	globalMask,
	FIFO_RECV_MISC,
	"ding"
	);

} /* end initialize */

/*===================================================
	myUsage - entry point
===================================================*/
void myUsage()
{
printf("======================= usage =====================================\n");
printf(" usage for fifoReceiver:\n");
printf("     fifoReceiver -n <myName> <optionals>\n");
printf("     where optionals are:\n");
printf("        -l <loggerName> - connect to trace logger\n");
printf("===================================================================\n");
}// end myUsage
