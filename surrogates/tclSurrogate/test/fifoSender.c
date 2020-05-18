/*======================================
	fifoSender.c

Description:
This module is used to send message to VC
which will relay it to socket connected to GUI.

-----------------------------------------------------------------------
    Copyright (C) 1998, 2002, 2007 FCSoftware Inc. 

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
 *$Log: fifoSender.c,v $
 *Revision 1.1  2009/01/19 20:16:02  bobfcsoft
 *relocated files
 *
 *Revision 1.2  2007/07/24 21:47:20  bobfcsoft
 *new contact info
 *
 *Revision 1.1.1.1  2005/03/27 11:50:49  paul_c
 *Initial import
 *
 *Revision 1.6  2002/11/22 16:34:06  root
 *2.0rc3
 *
 *Revision 1.5  2002/11/19 15:36:02  root
 *2.0rc1
 *
 *Revision 1.4  2002/06/12 18:33:28  root
 *fcipc merge completed
 *
 *Revision 1.3  2000/10/13 14:06:39  root
 *LGPL
 *
 *Revision 1.2  2000/02/15 18:55:55  root
 *fixed int main warning on 2.2 systems
 *
 *Revision 1.1  1999/11/04 15:42:01  root
 *Initial revision
 *
 *Revision 1.1  1999/07/13 18:11:49  root
 *Initial revision
 *
 *Revision 1.2  1999/06/30 01:34:05  root
 *added detachName
 *
 *Revision 1.1  1999/06/29 13:42:35  root
 *Initial revision
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/timeb.h>

#include "surroTestMsgs.h"

#define _ALLOC
#include "fifoSender.h"
#include "loggerVars.h"
#undef _ALLOC

#include "simplProto.h"
#include "loggerProto.h"
void initialize(int argc, char **argv);
void myUsage();

/*--------------------------------------
	fifoSender - entry point
--------------------------------------*/
int main(int argc, char **argv, char **envp)
{
static char *fn="fifoSender";
SURRO_TEST_MSG *outMsg;
SURRO_TEST_MSG *inMsg;
int n;

printf("%s:starting\n",fn);

initialize(argc, argv);

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

#if 0
loadSend(outArea,sizeof(SURRO_TEST_MSG));
if(sendMsgx(toPid) == -1)
#endif
n=Send(toPid,outArea,inArea,sizeof(SURRO_TEST_MSG),sizeof(SURRO_TEST_MSG));
if(n == -1)
	{
	printf("%s:got Send error\n",fn);
	fcLogx(__FILE__, fn,
		globalMask,
		FIFO_SENDER_MISC,
		"got Send error"
		);
	}
else
	{
#if 0
	n=retrieveReply(inArea);
#endif

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
	} // end if send error

printf("%s:done\n",fn);
name_detach();

return(1);

}// end fifoSender

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
toName[0]=0;
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

			case 'r':
                                for(;*p != 0; p++);
				sprintf(toName,"%s",++p);
				toPid=name_locate(toName);
				if(toPid == -1)
					{
					printf("%s: unable to locate <%s>\n",
						fn,
						toName);
					name_detach();
					exit(0);
					}
				break;

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
if(toName[0] == 0 || myMsg[0] == 0)
	{
	myUsage();
	name_detach();
	exit(0);
	}

logger_ID = is_logger_upx(loggerName);

fcLogx(__FILE__, fn,
	globalMask,
	FIFO_SENDER_MISC,
	"ding"
	);

} /* end initialize */

/*===================================================
	myUsage - entry point
===================================================*/
void myUsage()
{
printf("======================= usage =====================================\n");
printf(" usage for fifoSender:\n");
printf("     fifoSender -n <myName> -r <recv name> -m <message> <optionals>\n");
printf("     where optionals are:\n");
printf("        -l <loggerName> - connect to trace logger\n");
printf("===================================================================\n");
}// end myUsage
