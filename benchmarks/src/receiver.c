/*=============================================
	receiver.c

Description:
This file contains code for a simple "receiver"
task.  It is used to form the "receiver" portion
of the A->B "benchmark" tests.

-----------------------------------------------------------------------
    Copyright (C) 1998, 2002 FCSoftware Inc. 

    This software is in the public domain.
    Permission to use, copy, modify, and distribute this software and its
    documentation for any purpose and without fee is hereby granted, 
    without any conditions or restrictions.
    This software is provided "as is" without express or implied warranty.

    If you discover a bug or add an enhancement contact us on the
    SIMPL project mailing list. 

-----------------------------------------------------------------------
=============================================*/
/*
 *$Log: receiver.c,v $
 *Revision 1.5  2011/09/08 16:13:43  bobfcsoft
 *getopt version for Mac compatibility
 *
 *Revision 1.4  2011/03/23 16:48:08  bobfcsoft
 *expanded name size to allow for remote name attach
 *
 *Revision 1.3  2010/01/14 13:07:30  bobfcsoft
 *myName[0] in name check
 *
 *Revision 1.2  2007/07/24 20:08:45  bobfcsoft
 *new comment info
 *
 *Revision 1.1.1.1  2005/03/27 11:50:39  paul_c
 *Initial import
 *
 *Revision 1.11  2002/11/22 16:25:35  root
 *2.0rc3
 *
 *Revision 1.10  2002/11/19 15:01:02  root
 *2.0rc1
 *
 *Revision 1.9  2002/05/29 15:28:32  root
 *enhanced with _SIMPLFCIPC compatibility
 *
 *Revision 1.8  2001/07/05 14:41:05  root
 *added _SIMPLIPC case
 *
 *Revision 1.7  2001/01/29 15:11:48  root
 *using fcipcProto.h instead of ipcProto.h
 *
 *Revision 1.6  2000/10/13 13:51:49  root
 *LGPL
 *
 *Revision 1.5  2000/04/17 14:57:45  root
 *enhanced for SRR, SIPC and QNX
 *
 *Revision 1.4  2000/02/15 18:49:32  root
 *added detachName to end
 *fixed int main warning
 *
 *Revision 1.3  1999/09/09 13:55:43  root
 **** empty log message ***
 *
 *Revision 1.2  1999/03/29 16:07:07  root
 *bug fixes
 *
 *Revision 1.1  1999/02/04 15:43:20  root
 *Initial revision
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <getopt.h>
#include <sys/shm.h>

#define _ALLOC
#include "receiver.h"
#undef _ALLOC

#include "simplmiscProto.h"

#ifdef _FIFO
#include "fcipcProto.h"
#endif

#ifdef _SIMPLFCIPC
#include "simpl.h"
#include "simplProto.h"
#endif

#ifdef _SIPC
#include "sipc_lib.h"
#endif

#ifdef _SRR
#include "srrProto.h"
#endif

#ifdef _SIMPLIPC
#include "simpl.h"
#endif

void initialize(int, char **);

/*--------------------------------------------
	receiver - entry point
--------------------------------------------*/
int main(int argc, char **argv, char **envp)
{
static char *fn="receiver";
#ifdef _FIFO
int fromWhom;
int nbytes;
char *msg;
#endif
#ifdef _SIMPLFCIPC
int fromWhom;
int nbytes;
char *msg;
#endif
#ifdef _SIPC
int fromWhom;
int rbytes;
#endif
#ifdef _SRR
int fromWhom;
int rbytes;
#endif
#ifdef _SIMPLIPC
char * fromWhom;
int rbytes;
#endif

initialize(argc, argv);

printf("top of receive loop\n");
while(1)
	{
#ifdef _FIFO
	fromWhom = receiveMsg(&msg, &nbytes);
	loadReply(fromWhom,msg,nbytes);
	replyMsg(fromWhom);
#endif
#ifdef _SIMPLFCIPC
	fromWhom = receiveMsg(&msg, &nbytes);
	loadReply(fromWhom,msg,nbytes);
	replyMsg(fromWhom);
#endif
#ifdef _QNX
	fromWhom = Receive(0,inArea,1024);
	Reply(fromWhom, outArea, 1024);
#endif
#ifdef _SRR
	fromWhom = SrrReceive(0,inArea,&rbytes);
	SrrReply(fromWhom, outArea, rbytes);
#endif
#ifdef _SIPC
	fromWhom = Receive(inArea,&rbytes);
	Reply(fromWhom, outArea, rbytes);
#endif
#ifdef _SIMPLIPC
	rbytes = Receive(&fromWhom,inArea,8192);
	Reply(fromWhom, outArea, rbytes);
#endif
	}

printf("%s:done\n",fn);

#ifdef _FIFO
detachName();
#endif

#ifdef _SIMPLFCIPC
name_detach();
#endif

#ifdef _SRR
#endif

#ifdef _SIPC
name_detach();
#endif
#ifdef _SIMPLIPC
name_detach();
#endif

return(1);

}// end receiver

/*============================================
	initialize - entry point
============================================*/
void initialize(int argc, char **argv)
{
static char *fn="initialize";
//int i;                          /* loop variable */
int opt;	// for getopt version
char myName[128];

/*===============================================
  process command args
===============================================*/
#if 0
for(i=1; i<=argc; ++i)
        {
        char *p = argv[i];

        if(p == NULL) continue;

        if(*p == '-')
                {
                switch(*++p)
#endif
myName[0]=0;
while((opt = getopt(argc, argv, "n:")) != -1)
	{
	switch(opt)
                        {
                        case 'n':
#if 0
                                for(;*p != 0; p++);
				sprintf(myName,"%s",++p);
#endif
				sprintf(myName,"%s",optarg);
				
#ifdef _FIFO
				attachName(myName, 8192, NULL);
#endif

#ifdef _SIMPLFCIPC
				name_attach(myName,8192, NULL);
#endif

#ifdef _QNX
				qnx_name_attach(0, myName);
#endif
#ifdef _SRR
				nameid=SrrNameAttach(myName);
#endif
#ifdef _SIPC
				name_attach(myName,8192);
#endif
#ifdef _SIMPLIPC
				name_attach(myName, NULL);
#endif
				printf("Receiver <%s> starting\n",myName);
                                break;

                        default:
#if 0
				printf("%s:unknown arg %s\n",fn, p);
#endif
				printf("%s:unknown arg %c\n",fn, optopt);
                                break;
                        }/*end switch*/
#if 0
                } /* end if *p */
#endif
        }/*end for i*/

if(!myName[0])
	{
	printf("no name specified\n");
	printf("usage: receiver -n name\n");
	exit(0);
	}

#ifdef _FIFO
printf("FIFO version\n");
#endif
#ifdef _SIMPLFCIPC
printf("SIMPLFCIPC version\n");
#endif
#ifdef _SRR
printf("SRR version\n");
#endif
#ifdef _SIPC
printf("SIPC version\n");
#endif
#ifdef _SIMPLIPC
printf("SIMPLipc version\n");
#endif

printf("cksum=%lu\n",cksum(argv[0]));

} /* end initialize */
