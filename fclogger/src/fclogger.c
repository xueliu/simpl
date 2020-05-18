/*=============================================
	fclogger.c

Description:
This module can be run as a trace logger.  It uses
the FCipc library to become a "receiver".  Other 
processes can send it messages which it will write
to stdout.

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
=============================================*/
/*
 *$Log: fclogger.c,v $
 *Revision 1.3  2011/09/08 16:16:43  bobfcsoft
 *more portable arg processing
 *
 *Revision 1.2  2007/07/24 20:40:47  bobfcsoft
 *new contact info
 *
 *Revision 1.1.1.1  2005/03/27 11:50:45  paul_c
 *Initial import
 *
 *Revision 1.13  2002/11/22 16:27:51  root
 *2.0rc3
 *
 *Revision 1.12  2002/11/19 14:58:35  root
 *2.0rc1
 *
 *Revision 1.11  2002/06/12 18:20:29  root
 *fcipc merge completed
 *
 *Revision 1.10  2001/01/29 15:17:10  root
 *using fcipcProto.h instead of ipcProto.h
 *
 *Revision 1.9  2000/10/13 14:00:43  root
 *LGPL
 *
 *Revision 1.8  2000/02/15 18:45:31  root
 *fixed int main warning
 *
 *Revision 1.7  1999/11/04 15:35:07  root
 *cleanup
 *
 *Revision 1.6  1999/05/19 16:54:42  root
 *cleaned up warnings and
 *add NULL user exit function
 *
 *Revision 1.5  1999/02/04 14:43:14  root
 *cleanup
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/shm.h>

#include "simpl.h"
#include "simplDefs.h" //for FCMSG_REC

#include "simplProto.h"
void initialize(int, char **);

/*--------------------------------------------
	fclogger - entry point
--------------------------------------------*/
int main(int argc, char **argv, char **envp)
{
static char *fn="fclogger";
char * fromWhom;
int nbytes;
char *msg;

printf("fclogger starting\n");

initialize(argc, argv);

fflush(stdout);

while(1)
	{
#if 0
	fromWhom = receiveMsg(&msg, &nbytes);
#endif
	nbytes = Receive(&fromWhom, NULL, 1024);

	if(nbytes > 0)
		{
		FCMSG_REC *logMsg;

		logMsg=(FCMSG_REC *)fromWhom;
		msg=(char *)&logMsg->data;

		msg[nbytes]=0;

		printf("%.512s\n", msg);
		fflush(stdout);
	
		Reply(fromWhom, NULL, 0);
		}
	}

printf("%s:done\n",fn);

return(1);

}// end logger

/*============================================
	initialize - entry point
============================================*/
void initialize(int argc, char **argv)
{
static char *fn="initialize";
int i;                          /* loop variable */

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
//                                for(;*p != 0; p++);
				
//				name_attach(++p, NULL);
				name_attach(argv[++i], NULL);
                                break;

                        default:
				printf("%s:unknown arg %s\n",fn, p);
                                break;
                        }/*end switch*/
                } /* end if *p */
        }/*end for i*/

} /* end initialize */
